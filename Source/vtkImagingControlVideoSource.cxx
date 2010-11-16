/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImagingControlVideoSource.cxx
  Author:  Janne Beate Bakeng, SINTEF Health

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkImagingControlVideoSource.h"

#include "vtkObjectFactory.h"
#include "vtkCriticalSection.h"
#include "vtkTimerLog.h"
#include "vtkUnsignedCharArray.h"
#include "vtkMultiThreader.h"

//include support for writing to file
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkBMPWriter.h"
#include "vtkMetaImageWriter.h"

//include support for merging buffers
#include "vtkUnstructuredGrid.h"
#include "vtkImageAppend.h"

#include <limits>

//#include "tisudshl.h" included in header

vtkCxxRevisionMacro(vtkImagingControlVideoSource, "1.2");
//Using our own version of new because we are using the singelton design pattern.
//vtkStandardNewMacro(vtkImagingControlVideoSource);
//Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkImagingControlVideoSource);

vtkImagingControlVideoSource* vtkImagingControlVideoSource::Instance = 0;

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#  define vtkGetWindowLong GetWindowLongPtr
#  define vtkSetWindowLong SetWindowLongPtr
#  define vtkGWL_USERDATA GWLP_USERDATA
#else // regular Visual studio 
#  define vtkGetWindowLong GetWindowLong
#  define vtkSetWindowLong SetWindowLong
#  define vtkGWL_USERDATA GWL_USERDATA
#endif //

// Description:
// Helper class for listening for frame events from the grabber
class vtkImagingControlGrabberListener : public DShowLib::GrabberListener
{
public:
  // Overwrite the GrabberListener methods we need
virtual void frameReady( DShowLib::Grabber& caller, smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD currFrame )
  {
    //Copy the frame to the internal vtk buffer
    vtkImagingControlVideoSource::GetInstance()->InternalGrab(pBuffer, currFrame);
  }; 
  
  virtual void deviceLost( DShowLib::Grabber& caller )
  {
    vtkImagingControlVideoSource::GetInstance()->Stop();
  };
};

//----------------------------------------------------------------------------
vtkImagingControlVideoSource::vtkImagingControlVideoSource():
  IC_FRAMEBUFFER_SIZE(5)
{
  vtkDebugMacro("vtkImagingControlVideoSource::vtkImagingControlVideoSource()");

  //Setting some default vaules
  this->FrameIndex = 0;
  this->FrameSize[0] = 800;
  this->FrameSize[1] = 600;
  this->FrameSize[2] = 1;
  this->SetFrameBufferSize(1);
  this->FrameRate = 13; // in fps
  this->OutputFormat = VTK_LUMINANCE;
  this->NumberOfScalarComponents = 1;
  this->FrameBufferBitsPerPixel = 8;
  this->FlipFrames = 1;
  this->FrameBufferRowAlignment = 1;  

  //Setting some default IC variables
  this->ImagingControlLicenceKey = "0"; //Zero indicates trial version
  this->AcquisitionDataType = DShowLib::eRGB24;
}

//----------------------------------------------------------------------------
vtkImagingControlVideoSource::~vtkImagingControlVideoSource()
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::~vtkImagingControlVideoSource()");
  this->vtkImagingControlVideoSource::ReleaseSystemResources();
  delete Grabber;
}
//----------------------------------------------------------------------------
vtkImagingControlVideoSource* vtkImagingControlVideoSource::New()
{
  vtkImagingControlVideoSource* ret = vtkImagingControlVideoSource::GetInstance();
  ret->Register(NULL);
  return ret;
}
//----------------------------------------------------------------------------

// Return the single instance of the vtkOutputWindow
vtkImagingControlVideoSource* vtkImagingControlVideoSource::GetInstance()
{
  if(!vtkImagingControlVideoSource::Instance)
    {
    // Try the factory first
   vtkImagingControlVideoSource::Instance = (vtkImagingControlVideoSource*)vtkObjectFactory::CreateInstance("vtkImagingControlVideoSource");
   if(!vtkImagingControlVideoSource::Instance)
     {
     vtkImagingControlVideoSource::Instance = new vtkImagingControlVideoSource();   
     }
   if(!vtkImagingControlVideoSource::Instance)
     {
     std::cout << "Error, couldn't make a new Instance! " << std::endl;
     }
    }
  //Return the instance
  return vtkImagingControlVideoSource::Instance;
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::SetInstance(vtkImagingControlVideoSource* instance)
{
  if (vtkImagingControlVideoSource::Instance==instance)
    {
    return;
    }
  //Preferably this will be NULL
  if (vtkImagingControlVideoSource::Instance)
    {
    vtkImagingControlVideoSource::Instance->Delete();;
    }
  vtkImagingControlVideoSource::Instance = instance;
  if (!instance)
    {
    return;
    }
  //The user will call ->Delete() after setting instance
  instance->Register(NULL);
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::Initialize()
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::Initialize()");
  //Return if the the videosource already is initialized
  if (this->Initialized)
  {
    return;
  }

  vtkDebugMacro(<< "Setting the ImagingControlLicenceKey: " << this->ImagingControlLicenceKey);
  //1) initialize the IC library
  if( !DShowLib::InitLibrary( this->ImagingControlLicenceKey ) )
  {
    vtkErrorMacro(<< "Initialize: couldn't initialize the Imaging Control library, invalid license key?");
    this->ReleaseSystemResources();
    return;
  }

  //2) create a grabber
  this->Grabber = new DShowLib::Grabber();
  
  //3) set up a framehandlersink that contains the membuffercollection that receives teh grabbed images
  // Create a FrameTypeInfoArray data structure to describe the allowed color formats
  DShowLib::FrameTypeInfoArray acceptedTypes = DShowLib::FrameTypeInfoArray::createStandardTypesArray();

  // Create a frame handler sink
  vtkDebugMacro(<< "Setting the sink to contain " << this->FrameBufferSize << " buffers.");
  smart_ptr<DShowLib::FrameHandlerSink> sink = DShowLib::FrameHandlerSink::create(acceptedTypes, IC_FRAMEBUFFER_SIZE );

  // Apply the sink to the grabber
  this->Grabber->setSinkType(sink);

 //4) select a input device by showing the device settings page
  if(!this->Grabber->showDevicePage())
  {
    DShowLib::Error error = this->Grabber->getLastError();
    vtkErrorMacro(<< "Initialize: error when trying to show device page. (" << error.c_str() << "). Exiting.");
    this->ReleaseSystemResources();
    exit(1);
  }
  sink->setSnapMode(false);

  //Prepare the grabber
  if(!this->Grabber->prepareLive(false))
    vtkErrorMacro(<< "vtkImagingControlVideoSource::Initialize: Couldn't prepare the Grabber for live.");

  DShowLib::FrameTypeInfo info;
  sink->getOutputFrameType(info);
  DShowLib::tColorformatEnum sinkFormat = info.getColorformat();
  vtkDebugMacro(<< "FrameHandlerSink format set to: " << sinkFormat);

  //5)Set the callback function which gets invoked upon arrival of new frame
  vtkImagingControlGrabberListener *listener = new vtkImagingControlGrabberListener();
  this->Grabber->addListener(listener, DShowLib::GrabberListener:: eALL);

  //6)Setup the vtk buffer, also updates the framebuffer
  this->FrameBufferMutex->Lock();
  this->DoFormatSetup();
  this->FrameBufferMutex->Unlock();

  this->Initialized = 1;
  //Tell the system that this objects state has been altered
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::Record()
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::Record()");
  this->Initialize();
  if (!this->Initialized)
    {
    return;
    }

  if (this->Playing)
    {
    this->Stop();
    }

  if (!this->Recording)
    {
    this->Recording = 1;
    this->Modified();
    if(!this->Grabber->isLive())
      this->Grabber->startLive(false);
    }
}
    
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::Play()
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::Play()");
  
  this->vtkVideoSource::Play();
}
    
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::Stop()
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::Stop()");
  if (this->Recording)
  {
    this->Recording = 0;
    this->Modified();
  }
  else if (this->Playing)
  {
    this->vtkVideoSource::Stop();
  }
  if(this->Grabber->isLive())
  {
    this->Grabber->stopLive();
  }
}
//----------------------------------------------------------------------------
// Rewind back to the frame with the earliest timestamp.
void vtkImagingControlVideoSource::Rewind()
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::Rewind()");
  
  this->FrameBufferMutex->Lock();

  //Pointer to the timestamps
  double *stamp = this->FrameBufferTimeStamps;
  //The lowest timestamp found
  double lowest = 0;
  int i, j, k;

  //Setting lowest to highest possible value
  if (this->FrameBufferSize)
    {
      lowest = std::numeric_limits<double>::max();
    }
  //Going through the timestamps to find the oldest frame
  for (i = 0; i < this->FrameBufferSize; i++)
    {
    j = (this->FrameBufferIndex + i + 1) % this->FrameBufferSize;
    if (stamp[j] != 0.0 && stamp[j] <= lowest)
      {
      lowest = stamp[j];
      k = i;
      }
    }
  k++;
  j = (this->FrameBufferIndex + k) % this->FrameBufferSize;
  if (stamp[j] != 0.0 && stamp[j] < 980000000.0)
    {
    vtkWarningMacro("Rewind: bogus time stamp!");
    }
  else
    {
    //Rotate the buffer so it starts at the oldest frame
    this->AdvanceFrameBuffer(-k);
    this->FrameIndex = (this->FrameIndex - k) % this->FrameBufferSize;
    while (this->FrameIndex < 0)
      {
      this->FrameIndex += this->FrameBufferSize;
      }
    }
  this->FrameBufferMutex->Unlock();
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::Save(std::string filename, vtkImagingControlVideoSource::SaveAs saveAs, double syncFactor)
{
  //syncFactor is used to naivly synchronize this videostream with, for example a stream of positions gathered with
  //a tracking system. The idea is that we want the same amount of frames as positions. syncFactor=-1 is default and
  //indicates normal saving of all the frames.

  //Rewind the 'tape' so this->FrameBufferIndex points to the oldest frame
  this->Rewind();

  this->FrameBufferMutex->Lock();
  //Filter for appending alle the frames in the buffer to one volume
  vtkImageAppend* append = vtkImageAppend::New();
  //Z-axis == 2
  append->SetAppendAxis(2);

  //Find the FrameIndex of the first frame
  int firstFrameIndex = this->FrameBufferIndex;
 
  //Find the number of valid frames in the buffer
  double numberOfValidFrames = (this->FrameCount < this->FrameBufferSize) ? this->FrameCount : this->FrameBufferSize;
  
  //Calculate the lag factor
  double lagFactor = (syncFactor == -1) ? 1.0 : (numberOfValidFrames/syncFactor);

  int frameIndex;
  int filesToWrite = (syncFactor == -1) ? numberOfValidFrames : syncFactor;

  //Opening a file for writing timestamp info
  std::ofstream timestamps;
  timestamps.open("timestamps.txt");
  //Number of frames, also specifies that the number should be written as fixed
  timestamps << std::fixed << filesToWrite << " ";

  for(unsigned int i=0; i<filesToWrite; i++)
  {
    //Calculate what frame to append
    int offset = ((int)(this->FrameBufferIndex-(i*lagFactor)));
    if(offset <= 0)
    {
      offset = this->FrameBufferSize + offset;
    }
    frameIndex = offset%this->FrameBufferSize;

    //Pointer to the  internal buffer
    vtkUnsignedCharArray* frame = (reinterpret_cast<vtkUnsignedCharArray*>(this->FrameBuffer[frameIndex]));
    
    //Writing timestampinfo to the timestamp file
    timestamps << this->FrameBufferTimeStamps[frameIndex] << " ";
    vtkDebugMacro(<< "Appended frame has timestamp " << this->FrameBufferTimeStamps[frameIndex]);
    
    //Setting up the imagedata
    vtkImageData* image = vtkImageData::New();
    image->GetPointData()->SetScalars(frame);
    image->SetDimensions(this->FrameSize);
    image->SetScalarTypeToUnsignedChar();
    image->SetNumberOfScalarComponents(this->NumberOfScalarComponents);
    image->SetSpacing(this->DataSpacing[0], this->DataSpacing[1], this->DataSpacing[2]);
    image->SetOrigin(this->DataOrigin[0], this->DataOrigin[1], this->DataOrigin[2]);
    
    //Adding the imagedata as input to the append filter
    append->AddInput(image);
    
    //Update the output of the append filter
    append->Update();
  }

  //Close the timestampsfile
  timestamps.close();
  
  //Create the writer
  void* writer;
  
  switch (saveAs)
  {
    case vtkImagingControlVideoSource::BMP:
      //Write as a series of bitmaps
      this->Update();
      writer = vtkBMPWriter::New();
      ((vtkBMPWriter*)writer)->SetInputConnection(append->GetOutputPort());
      ((vtkBMPWriter*)writer)->SetFilePrefix(filename.c_str());
      ((vtkBMPWriter*)writer)->SetFilePattern("%s%03d.bmp");
      ((vtkBMPWriter*)writer)->Update();
      ((vtkBMPWriter*)writer)->Write();
      ((vtkBMPWriter*)writer)->Delete();
      break;
    case vtkImagingControlVideoSource::RAW:
      //Write as a metaheader + raw file
      this->Update();
      writer = vtkMetaImageWriter::New();
      ((vtkMetaImageWriter*)writer)->SetInputConnection(append->GetOutputPort());
      ((vtkMetaImageWriter*)writer)->SetFileDimensionality(3);
      ((vtkMetaImageWriter*)writer)->SetFileName("MetaFile.mdh");
      ((vtkMetaImageWriter*)writer)->SetRAWFileName("RawFile.raw");
      ((vtkMetaImageWriter*)writer)->Update();
      ((vtkMetaImageWriter*)writer)->Write();
      ((vtkMetaImageWriter*)writer)->Delete();
      break;
  }
    append->Delete();
    this->FrameBufferMutex->Unlock();
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::Grab()
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::Grab()");
  std::cout << "vtkImagingControlVideoSource::Grab()" << std::endl;

  //Stop recording or playing
  if (this->Recording || this->Playing)
    {
    this->Stop();
    }

  //Set the framebuffersize to one
    this->SetFrameBufferSize(1);

  // ensure that the frame buffer is properly initialized
  this->Initialize();
  if (!this->Initialized)
    {
    return;
    }

  //Start Recording to simulate a preview of the videostream
  this->Record();
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::InternalGrab( smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD currFrame)
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::InternalGrab()");
  // 1) Do frame buffer indices maintenance
  // 2) Do time stamping
  // 3) Decode the data according to type
  // 4) Copy the data to the local vtk frame buffer

  this->FrameBufferMutex->Lock();

  // 1 ) Do frame buffer indices maintanance
  if (this->AutoAdvance)
    {
    this->AdvanceFrameBuffer(1);
    if (this->FrameIndex + 1 < this->FrameBufferSize)
      {
      this->FrameIndex++;
      }
    }
  int index = this->FrameBufferIndex;

  // 2)Do the time stamping
  this->FrameBufferTimeStamps[index] = vtkTimerLog::GetUniversalTime();
  vtkDebugMacro(<< "Doing timestamp: " << this->FrameBufferTimeStamps[index]);
  std::cout << "Doing timestamp for frame  " << index << std::endl;

  if(this->FrameCount++ == 0)
  {
    this->StartTimeStamp = this->FrameBufferTimeStamps[index];
  }

  //3) Decode the data according to type

  //get info on how to interpret the incoming data
  const DShowLib::FrameTypeInfo frameInfo = pBuffer->getFrameType();
  const SIZE dimensionOfTheFrame = frameInfo.dim;

  //get the pointer to the actual incoming data on to a local pointer
  unsigned char* frameBufferPtr = (unsigned char*) pBuffer->getPtr();

  //get the pointer to the correct location in the local frame buffer, where the data should be stored
  unsigned char* vtkBufferPtr = (unsigned char*)((reinterpret_cast<vtkUnsignedCharArray*>(this->FrameBuffer[index]))->GetPointer(0));
  
  //4) Copy the data from the incoming frame to the vtk buffer

  //Find out how many bytes per row the frameBuffer and vtkBuffer can take
  const bool dataIsStoredBottomUp = frameInfo.isBottomUpFrameType();

  //Calculate the index of the upper left pixel in the framebuffer
  //Images are stored upside down in the framebuffer
  const int iOffsetUpperLeft = (dimensionOfTheFrame.cy -1) * dimensionOfTheFrame.cx;
  const int frameBufferNumberOfRows = dimensionOfTheFrame.cy;
  const int vtkBufferNumberOfRows = this->FrameSize[1];

  if(frameBufferNumberOfRows == vtkBufferNumberOfRows)
  {
    if(dataIsStoredBottomUp)
    {
      //for every row in the image, unpack it to rgb and copy it
      //into the vtkBuffer
      for(int i = 0; i<frameBufferNumberOfRows ; i++)
      {
        //the start of each row
        const int start = iOffsetUpperLeft - i*dimensionOfTheFrame.cx;
        unsigned char* outPtr = new unsigned char[dimensionOfTheFrame.cx*(this->NumberOfScalarComponents)*8];
        this->UnpackRasterLine(outPtr, frameBufferPtr, start, dimensionOfTheFrame.cx);

        //copy row by row into the vtkBuffer
        memcpy(vtkBufferPtr, outPtr, dimensionOfTheFrame.cx*this->NumberOfScalarComponents);
        vtkBufferPtr += dimensionOfTheFrame.cx*this->NumberOfScalarComponents;
        delete outPtr;
      }
    }
    else
    {
      vtkErrorMacro(<< "vtkImagingControlVideoSource::InternalGrab: Data isn't stored bottom up! What to do? Exiting.");
      exit(0);
    }
  }
  else
  {
    vtkErrorMacro(<< "vtkImagingControlVideoSource::InternalGrab: Not the same number of bytes in the frameBuffer as in the vtkBuffer! Exiting");
    exit(0);
  }
  //Telling the system that this object has been modified
  this->Modified();

  this->FrameBufferMutex->Unlock();
}
//----------------------------------------------------------------------------
// Check the current video format and set up the VTK video framebuffer to match
void vtkImagingControlVideoSource::DoFormatSetup()
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::DoFormatSetup()");
  DShowLib::VideoFormatItem videoFormatItem = this->Grabber->getVideoFormat();
  
  // set the frame size
  vtkDebugMacro(<< "VideoFormatItem::getSize = " << videoFormatItem.getSize().cx << ", " << videoFormatItem.getSize().cy);
  this->FrameSize[0] = videoFormatItem.getSize().cx; //width of a frame
  this->FrameSize[1] = videoFormatItem.getSize().cy; //height of a frame
  
  //set number of bits used to represent a pixel
  vtkDebugMacro(<< "VideoFormatItem::getBitsPerPixel = " << videoFormatItem.getBitsPerPixel());
  this->FrameBufferBitsPerPixel = videoFormatItem.getBitsPerPixel();

  //Set the AcquisitionDataType
  this->AcquisitionDataType = DShowLib::getColorformatFromSubtype(videoFormatItem.getColorformat());
  vtkDebugMacro(<< "Setting this->AcquisitionDataType to " << this->AcquisitionDataType);

  //Set the sinks datatype
  this->SinkDataType = ((smart_ptr<DShowLib::FrameHandlerSink>)(this->Grabber->getSinkTypePtr()))->getMemBufferCollection()->getColorformat();
  vtkDebugMacro(<< "Setting this->SinkDataType to " << this->SinkDataType);

  switch (this->AcquisitionDataType)
  {
  case DShowLib::eRGB8:
  case DShowLib::eY800:
    this->SetOutputFormat(VTK_LUMINANCE);
    break;
  case DShowLib::eUYVY:
  case DShowLib::eRGB24:
    this->SetOutputFormat(VTK_RGB);
    break;
  case DShowLib::eRGB32:
    this->SetOutputFormat(VTK_RGBA);
    break;
  case DShowLib::eRGB565:
  case DShowLib::eRGB555:
  case DShowLib::eYGB0:
  case DShowLib::eYGB1:
  case DShowLib::eBY8:
  default:
    vtkErrorMacro(<< "Unrecognized AcquisitionDataType in vtkImagingControlVideoSource::DoFormatSetup");
  }
  
  //Set the framerate
  if(this->Grabber->getFPS() != this->FrameRate)
  {
    this->SetFrameRate(this->Grabber->getFPS());
  }

  this->UpdateFrameBuffer();

  //Tell the system that this object has been modified
  this->Modified();
}
 //----------------------------------------------------------------------------
// The UnpackRasterLine method should be overridden if the framebuffer uses
// unusual pixel packing formats, such as XRGB XBRG BGRX BGR etc.
void vtkImagingControlVideoSource::UnpackRasterLine(unsigned char *outptr, unsigned char *inptr,int start, int count)
{
  switch (this->SinkDataType)
  {
  case DShowLib::eY800://VTK_LUMINANCE
  case DShowLib::eRGB8:
    this->BYTEToVTKLuminance(reinterpret_cast<BYTE*>(outptr), (BYTE*) inptr, start, count);
    break;
  case DShowLib::eRGB24: //VTK_RBG
    this->RGB24ToVTKRGB(reinterpret_cast<RGB24Pixel*>(outptr), reinterpret_cast<RGB24Pixel*>(inptr), start, count);
    break;
  case DShowLib::eRGB32://VTK_RGBA
    if(this->AcquisitionDataType == DShowLib::eRGB32)
    {
      this->RGB32ToVTKRGBA(reinterpret_cast<RGB32Pixel*>(outptr), (RGB32Pixel*) inptr, start, count);
    }
    else if(this->AcquisitionDataType == DShowLib::eRGB8)
    {
      this->RGB32ToVTKLuminance(reinterpret_cast<BYTE*>(outptr), (RGB32Pixel*) inptr, start, count);
    }
    else if(this->AcquisitionDataType == DShowLib::eUYVY)
    {
      this->UYVYToVTKRGB(reinterpret_cast<RGB24Pixel*>(outptr), reinterpret_cast<UYVY*>(inptr), start, count);
    }
    break;
  case DShowLib::eBY8:
  case DShowLib::eRGB565:
  case DShowLib::eRGB555:
  case DShowLib::eYGB0:
  case DShowLib::eYGB1:
  case DShowLib::eUYVY:
  default:
    vtkErrorMacro(<< "Unhandlet sink format found: " << this->SinkDataType);
    break;
  }
}
void vtkImagingControlVideoSource::BYTEToVTKLuminance(BYTE *outPtr, BYTE *rowPtr,  int start, int count)
{
    for(unsigned int i=0; i<count; i++)
    {
      outPtr[i] = rowPtr[start+i];
    }
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::RGB24ToVTKRGB(RGB24Pixel *outPtr, RGB24Pixel *rowPtr, int start, int count)
{
    for(unsigned int i=0; i<count; i++)
    {
      outPtr[i].r = rowPtr[start+i].b;
      outPtr[i].g = rowPtr[start+i].g;
      outPtr[i].b = rowPtr[start+i].r;
    }
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::RGB32ToVTKRGBA(vtkImagingControlVideoSource::RGB32Pixel *outPtr, vtkImagingControlVideoSource::RGB32Pixel *rowPtr, int start, int count)
{
  for(unsigned int i=0; i<count; i++)
  {
    outPtr[i].r = rowPtr[start+i].b;
    outPtr[i].g = rowPtr[start+i].g;
    outPtr[i].b = rowPtr[start+i].r;
    outPtr[i].a = rowPtr[start+i].a;
  }
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::RGB32ToVTKLuminance(BYTE *outPtr, vtkImagingControlVideoSource::RGB32Pixel *rowPtr, int start, int count)
{
  for(unsigned int i=0; i<count; i++)
  {
    outPtr[i] = rowPtr[start+i].b;
  }
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::UYVYToVTKRGB(vtkImagingControlVideoSource::RGB24Pixel *outPtr, vtkImagingControlVideoSource::UYVY *rowPtr, int start, int count)
{
  for(unsigned int i=0; i<count; i++)
  {
    /**
    B = 1.164(Y - 16) + 2.018(U - 128)
    G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
    R = 1.164(Y - 16) + 1.596(V - 128)
    */
    
    //TODO: Clamp the values, and fix the image
    outPtr[i].b = this->Clamp(1.164*(rowPtr[start+i].y1-16) + 1.596*(rowPtr[start+i].v-128));
    outPtr[i].g = this->Clamp(1.164*(rowPtr[start+i].y1-16) - 0.813*(rowPtr[start+i].v-128) - 0.391*(rowPtr[start+i].u-128));
    outPtr[i].r = this->Clamp(1.164*(rowPtr[start+i].y1-16) + 2.018*(rowPtr[start+i].u-128));
  }
}
unsigned char vtkImagingControlVideoSource::Clamp(unsigned char value)
{
  //Only need to clamp to 24 bit atm
  if(value > 255)
    value=255;

  if(value < 0)
    value=0;

  return value;
}
//----------------------------------------------------------------------------
// Check the current video format and set up the VTK video framebuffer to match
void vtkImagingControlVideoSource::SetOutputFormat(int format)
{
  vtkDebugMacro(<< "vtkImagingControlVideoSource::SetOutputFormat()");

  // convert color format to number of scalar components
  int numberOfComponents;

  switch (format)
  {
  case VTK_RGBA:
    this->OutputFormat = VTK_RGBA;
    numberOfComponents = 4;
    break;
  case VTK_RGB:
    this->OutputFormat = VTK_RGB;
    numberOfComponents = 3;
    break;
  case VTK_LUMINANCE:
    this->OutputFormat = VTK_LUMINANCE;
    numberOfComponents = 1;
    break;
  default:
    numberOfComponents = 0;
    vtkErrorMacro(<< "SetOutputFormat: Unrecognized color format.");
  }
  this->NumberOfScalarComponents = numberOfComponents;

  if (this->FrameBufferBitsPerPixel != numberOfComponents*8)
  {
    this->FrameBufferMutex->Lock();
    this->FrameBufferBitsPerPixel = numberOfComponents*8;
    if (this->Initialized)
    {
      this->UpdateFrameBuffer();    
    }
    this->FrameBufferMutex->Unlock();
  }

  this->Modified();
}
//----------------------------------------------------------------------------

void vtkImagingControlVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "NumberOfScalarComponents: " << this->NumberOfScalarComponents << "\n";

  os << indent << "Licence: " << this->ImagingControlLicenceKey << "\n";
  os << indent << "AcquisitionDataType: " << this->AcquisitionDataType << "\n";
  os << indent << "SinkDataType: " << this->SinkDataType << "\n";
}
//----------------------------------------------------------------------------
void vtkImagingControlVideoSource::SetImagingControlLicenceKey(char *ImagingControlLicenceKey)
{
  vtkDebugMacro("vtkImagingControlVideoSource::SetImagingControlLicenceKey()");
  if(ImagingControlLicenceKey)
    {
    this->ImagingControlLicenceKey = new char[256];
    sprintf(this->ImagingControlLicenceKey, "%s", ImagingControlLicenceKey);
    }
}

