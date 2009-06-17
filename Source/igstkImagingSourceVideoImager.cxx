/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagingSourceVideoImager.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif


#include "igstkImagingSourceVideoImager.h"
#include "igstkEvents.h"
#include "vtkImageData.h"
#include <itksys/SystemTools.hxx>

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"

#define MAX_DEVICES 64
#define MAX_FORMATS 64

namespace igstk
{

//Initialize static variables
std::map< unsigned int, std::string> ImagingSourceVideoImager::m_ErrorCodeContainer;
bool ImagingSourceVideoImager::m_ErrorCodeListCreated = false;

/** Constructor: Initializes all internal variables. */
ImagingSourceVideoImager::ImagingSourceVideoImager(void):m_StateMachine(this)
{
  this->m_NumberOfTools = 0;

  this->SetThreadingEnabled( true );

  // Lock for the data buffer that is used to transfer the
  // frames from thread that is communicating
  // with the imager to the main thread.
  m_BufferLock = itk::MutexLock::New();

  // Create error code list if it hasn't been
  // created.
  if( ! m_ErrorCodeListCreated )
    {
    this->CreateErrorCodeList();
    m_ErrorCodeListCreated = true;
    }
}

/** Destructor */
ImagingSourceVideoImager::~ImagingSourceVideoImager(void)
{

}

/** Create a map data structure containing MTC error code and description */
void ImagingSourceVideoImager::CreateErrorCodeList()
{
  ErrorCodeContainerType & ecc = m_ErrorCodeContainer;

  ecc[0]  = "OK";
  ecc[1]  = "Invalid object handle";
  ecc[2]  = "Reentrant access - library is not thread-safe";
  ecc[3]  = "Internal ImagingSourceVideoImager software error";
  ecc[4]  = "Null pointer parameter";
  ecc[5]  = "Out of memory";
  ecc[6]  = "Parameter out of range";
  ecc[7]  = "String parameter too long";
  ecc[8]  = "Insufficient space allocated by the client to the output buffer";
  ecc[9]  = "Camera not initialized";
  ecc[10] = "Camera already initialized - cannot be initialized twice";
  ecc[11] = "Camera initialization failed";
  ecc[12] = "MTC is incompatible with a software module it calls";
  ecc[13] = "Calibration file error: unrecognized camera model";
  ecc[14] = "Path not set";
  ecc[15] = "Cannot access the directory specified";
  ecc[16] = "Write to file failed";
  ecc[17] = "Invalid Index parameter";
  ecc[18] = "Invalid SideI parameter";
  ecc[19] = "Invalid Divisor parameter";
  ecc[20] = "Attempting to access an item of an empty IntCollection";
  ecc[21] = "Insufficient samples";
  ecc[22] = "Insufficient samples that fit within the acceptance tolerance";
  ecc[23] = "Odd number of vector samples";
  ecc[24] = "Less than 2 vectors";
  ecc[25] = "More than maximum vectors per facet";
  ecc[26] = "Error exceeds tolerance";
  ecc[27] = "Insufficient angle between vectors";
  ecc[28] = "First vector is shorter than the second";
  ecc[29] = "Vector lengths are too similar";
  ecc[30] = "Template vector has 0 length";
  ecc[31] = "The template has not been created or loaded";
  ecc[32] = "Template file is corrupt";
  ecc[33] = "Maximum number of marker templates allowed exceeded";
  ecc[34] = "Geometries of different facets are too similar";
  ecc[35] = "Noncompliant facet definition";
  ecc[36] = "The SampledVectorPairsCollection contains non-Vector handles";
  ecc[37] = "Empty pixels buffer";
  ecc[38] = "Dimensions do not match";
  ecc[39] = "File open failed";
  ecc[40] = "File read failed";
  ecc[41] = "File write failed";
  ecc[42] = "Cannot open calibration file "
            "(typically named [driver]_[ser num].calib";
  ecc[43] = "Not a calibration file";
  ecc[44] = "Calibration file contents corrupt";
  ecc[45] = "Calibration file was not generated from this camera";
  ecc[46] = "Calibration file not loaded";
  ecc[47] = "Incorrect file version";
  ecc[48] = "Input image location is out of bounds of the measurement volume";
  ecc[49] = "Input image locations do not triangulate to a valid 3-D point";
  ecc[50] = "Transform between coordinate spaces is unknown";
  ecc[51] = "The given camera object was not found in the cameras array";
  ecc[52] = "Feature Data unavailable for the current frame";
  ecc[53] = "Feature Data is corrupt or incompatible with the current version";
  ecc[54] = "XYZ position is outside of calibrated field of view";
  ecc[55] = "Grab frame error";
}

std::string
ImagingSourceVideoImager::GetErrorDescription( unsigned int code )
{
  if ( code >= 0 && code <= 55 )
    {
    return ImagingSourceVideoImager::m_ErrorCodeContainer[code];
    }
  else
    {
    return "Unknown error code";
    }
}

ImagingSourceVideoImager::ResultType ImagingSourceVideoImager::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::ImagingSourceVideoImager::InternalOpen called ...\n");

  if( ! this->Initialize() )
    {
    igstkLogMacro( CRITICAL, "Error initializing");
    return FAILURE;
    }

  return SUCCESS;
}

/** Initialize socket */
bool ImagingSourceVideoImager::Initialize( void )
{
  igstkLogMacro( DEBUG, "igstk::ImagingSourceVideoImager::Initialize called ...\n");

  bool result = true;

  return result;
}

/** Verify imager tool information. */
ImagingSourceVideoImager::ResultType
ImagingSourceVideoImager
::VerifyVideoImagerToolInformation( const VideoImagerToolType * imagerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagingSourceVideoImager::VerifyVideoImagerToolInformation called ...\n");

  return SUCCESS;
}

/** Detach camera. */
ImagingSourceVideoImager::ResultType ImagingSourceVideoImager::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::ImagingSourceVideoImager::InternalClose called ...\n");

  /*
    Close the device

    This invalidates the handle
  */
  if( !SUCCESS( unicap_close( m_Handle ) ) )
  {
     fprintf( stderr, "Failed to close the device\n");
  }
  else
  {
       fprintf( stderr, "Close the device\n");
  }
  return SUCCESS;
}

/** Put the imaging device into imaging mode. */
ImagingSourceVideoImager::ResultType ImagingSourceVideoImager::InternalStartImaging( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagingSourceVideoImager::InternalStartImaging called ...\n");

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine)

  unsigned int times = 0;
  unsigned int maxTimes = 20;

  bool deviceOpen = false;
  int i;
  do
  {
    times++;

    igstk::DoubleTypeEvent evt;
    evt.Set( (double)times/(double)maxTimes );
    this->InvokeEvent( evt );

    /**ImaginsSourceConverter begin*/

    m_Handle = open_device();
    if(!m_Handle)
    {
      fprintf( stderr, "Failed to get handle %d\n", i );
    }
    set_format(m_Handle);
    deviceOpen = true;

    if( !SUCCESS( unicap_get_format( m_Handle, &m_Format ) ) )
    {
       fprintf( stderr, "Failed to get video format!\n" );
       exit( -1 );
    }

    m_Format.buffer_type = UNICAP_BUFFER_TYPE_USER;

    if( !SUCCESS( unicap_set_format( m_Handle, &m_Format ) ) )
    {
       fprintf( stderr, "Failed to set video format!\n" );
       exit( -1 );
    }

     m_Buffer.data = (unsigned char*)malloc( m_Format.buffer_size );
     m_Buffer.buffer_size = m_Format.buffer_size;

     unicap_start_capture( m_Handle );
     unicap_queue_buffer( m_Handle, &m_Buffer );

     returned_buffer = new unicap_data_buffer_t;
     returned_buffer->data = (unsigned char*)malloc( m_Format.buffer_size );
     returned_buffer->buffer_size = m_Format.buffer_size;

  } while ( (times<maxTimes) && ( !deviceOpen ) );

  if ( times>=10 )
  {
    std::cout << " Could not find a client!" << std::endl;
    return FAILURE;
  }
  else
  {
    igstk::DoubleTypeEvent evt;
    evt.Set( 1.0 );
    this->InvokeEvent( evt );
    std::cout << "Start capture on device!" << std::endl;
  }
  return SUCCESS;
}

/** Take the imaging device out of imaging mode. */
ImagingSourceVideoImager::ResultType ImagingSourceVideoImager::InternalStopImaging( void )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagingSourceVideoImager::InternalStopImaging called ...\n");
  /*
    Stop the device
  */
  if( !SUCCESS( unicap_stop_capture( m_Handle ) ) )
  {
     fprintf( stderr, "Failed to stop capture device\n");
  }

  free( m_Buffer.data );
  delete returned_buffer;

  return SUCCESS;
}

/** Reset the imaging device to put it back to its original state. */
ImagingSourceVideoImager::ResultType ImagingSourceVideoImager::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::ImagingSourceVideoImager::InternalReset called ...\n");
  return SUCCESS;
}


/** Update the status and the transforms for all VideoImagerTools. */
ImagingSourceVideoImager::ResultType ImagingSourceVideoImager::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG,
    "igstk::ImagingSourceVideoImager::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Imaging state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  typedef VideoImagerToolFrameContainerType::const_iterator  InputConstIterator;

  InputConstIterator inputItr = this->m_ToolFrameBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolFrameBuffer.end();

  VideoImagerToolsContainerType imagerToolContainer =
  this->GetVideoImagerToolContainer();

  unsigned int toolId = 0;

  while( inputItr != inputEnd )
    {
    // only report tools that have useful data
    if (! this->m_ToolStatusContainer[inputItr->first])
    {
      igstkLogMacro( DEBUG, "igstk::ImagingSourceVideoImager::InternalUpdateStatus: " <<
              "tool " << inputItr->first << " is not in view\n");
      // report to the imager tool that the imager is not available
      this->ReportImagingToolNotAvailable(
        imagerToolContainer[inputItr->first]);

      ++inputItr;
      continue;
    }

    // report to the imager tool that the tool is Streaming
    this->ReportImagingToolStreaming(imagerToolContainer[inputItr->first]);

    this->SetVideoImagerToolFrame(
          imagerToolContainer[inputItr->first], (inputItr->second) );

    this->SetVideoImagerToolUpdate(
      imagerToolContainer[inputItr->first], true );

    ++inputItr;
    ++toolId;
    }

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the shared memory buffer and the tool's internal frame. This function
 *  is called by the thread that communicates with the imager while
 *  the imager is in the Imaging state. */
ImagingSourceVideoImager::ResultType
ImagingSourceVideoImager::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::ImagingSourceVideoImager::InternalThreadedUpdateStatus called ...\n");

  cout << "<>" << endl;
  cout << "<Update frame in InternalThreadedUpdateStatus>" << endl;
  cout << "<>" << endl;

  // Lock the buffer that this method shares with InternalUpdateStatus
  m_BufferLock->Lock();

  //reset the status of all the imager tools
  typedef VideoImagerToolFrameContainerType::const_iterator  InputConstIterator;
  InputConstIterator inputItr = this->m_ToolFrameBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolFrameBuffer.end();

  while( inputItr != inputEnd )
  {
    this->m_ToolStatusContainer[inputItr->first] = 0;
    ++inputItr;
  }

  try
  {
    igstkLogMacro( DEBUG, "InternalThreadedUpdateStatus Receive passed" );
    // Check if an imager tool was added with this device name
    typedef VideoImagerToolFrameContainerType::iterator InputIterator;

    //TODO toolname hard coded
    InputIterator deviceItr = this->m_ToolFrameBuffer.find( "Camera" );

    if( deviceItr != this->m_ToolFrameBuffer.end() )
    {
      // create the frame
      VideoImagerToolsContainerType imagerToolContainer =
                                              this->GetVideoImagerToolContainer();
//TODO commented due to higher frame rates
//      // image dimension set on tools
//      unsigned int toolDims[3];
//      imagerToolContainer[deviceItr->first]->GetFrameDimensions(toolDims);
//
//      unsigned int toolSize = toolDims[0] * toolDims[1] * toolDims[2];
//
//      if (buffer.buffer_size != toolSize)
//      {
//        igstkLogMacro( CRITICAL, "Incoming image size does not match with expected" );
//        m_BufferLock->Unlock();
//        return FAILURE;
//      }

      if( !SUCCESS( unicap_wait_buffer( m_Handle, &returned_buffer ) ) )
      {
      fprintf( stderr, "Failed to wait for buffer!\n" );
      }

      if( !SUCCESS( unicap_queue_buffer( m_Handle, returned_buffer ) ) )
      {
      fprintf( stderr, "Failed to queue buffer!\n" );
      }

      //this->m_ToolFrameBuffer[ deviceItr->first ] = frame;
      FrameType* frame = new FrameType();
      frame = this->GetVideoImagerToolFrame( imagerToolContainer[deviceItr->first]);
      uyvy2rgb24(
         (unsigned char*)frame->GetImagePtr(),//data pointer in frame, new buffer dest
         returned_buffer->data,//buffer with frame from device (UYVY)
         m_Format.size.width * m_Format.size.height * 3, // size of frame in RGB
         m_Format.size.width * m_Format.size.height * 2  // size of frame in UYVY
         //width * height * 2 ( 1 Y, 1/2 Cr , 1/2 Cb )
         );

     //update frame validity time
     frame->SetTimeToExpiration(this->GetValidityTime());

     this->m_ToolFrameBuffer[ deviceItr->first ] = frame;
     this->m_ToolStatusContainer[ deviceItr->first ] = 1;
     }
     m_BufferLock->Unlock();
     return SUCCESS;
  }
  catch(...)
  {
    igstkLogMacro( CRITICAL, "Unknown error catched" );
    m_BufferLock->Unlock();
    return FAILURE;
  }
}

ImagingSourceVideoImager::ResultType
ImagingSourceVideoImager::
AddVideoImagerToolToInternalDataContainers( const VideoImagerToolType * imagerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagingSourceVideoImager::RemoveVideoImagerToolFromInternalDataContainers "
                 "called ...\n");

  if ( imagerTool == NULL )
  {
    return FAILURE;
  }

  const std::string imagerToolIdentifier =
                  imagerTool->GetVideoImagerToolIdentifier();

  igstk::Frame* frame;

  this->m_ToolFrameBuffer[ imagerToolIdentifier ] = frame;
  this->m_ToolStatusContainer[ imagerToolIdentifier ] = 0;

  return SUCCESS;
}


ImagingSourceVideoImager::ResultType
ImagingSourceVideoImager::
RemoveVideoImagerToolFromInternalDataContainers
( const VideoImagerToolType * imagerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::ImagingSourceVideoImager::RemoveVideoImagerToolFromInternalDataContainers "
                 "called ...\n");

  const std::string imagerToolIdentifier =
                      imagerTool->GetVideoImagerToolIdentifier();

  // remove the tool from the frame buffer and status container
  this->m_ToolStatusContainer.erase( imagerToolIdentifier );
  this->m_ToolFrameBuffer.erase( imagerToolIdentifier );

  return SUCCESS;
}

/**The "ValidateSpecifiedFrequency" method checks if the specified
  * frequency is valid for the imaging device that is being used. */
ImagingSourceVideoImager::ResultType
ImagingSourceVideoImager::ValidateSpecifiedFrequency( double frequencyInHz )
{
  const double MAXIMUM_FREQUENCY = 30;
  if ( frequencyInHz < 0.0 || frequencyInHz > MAXIMUM_FREQUENCY )
    {
    return FAILURE;
    }
  return SUCCESS;
}


/** Print Self function */
void ImagingSourceVideoImager::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << " output Imaging Source Converter parameters " << std::endl;
}

unicap_handle_t
ImagingSourceVideoImager::open_device ()
{
  int dev_count;
  int status = STATUS_SUCCESS;
  unicap_device_t devices[MAX_DEVICES];
  unicap_handle_t m_Handle;
  int d = -1;

  for (dev_count = 0; SUCCESS (status) && (dev_count < MAX_DEVICES);
       dev_count++)
  {
    status =
  unicap_enumerate_devices (NULL, &devices[dev_count], dev_count);
    if (SUCCESS (status))
  {
    printf ("%d: %s\n", dev_count, devices[dev_count].identifier);
  }
    else
  {
    break;
  }
  }

  if (dev_count == 0)
  {
    // no device selected
    return NULL;
  }

  while ((d < 0) || (d >= dev_count))
  {
    //TODO get from config
  printf ("Open Device: ");
    //scanf ("%d", &d);
    d=0;
  }

  unicap_open (&m_Handle, &devices[d]);

  return m_Handle;
}

void
ImagingSourceVideoImager::set_format (unicap_handle_t m_Handle)
{
  unicap_format_t formats[MAX_FORMATS];
  int format_count;
  unicap_status_t status = STATUS_SUCCESS;
  int f = -1;

  for (format_count = 0; SUCCESS (status) && (format_count < MAX_FORMATS);
       format_count++)
  {
    status = unicap_enumerate_formats (m_Handle, NULL, &formats[format_count],  // (1)
           format_count);
    if (SUCCESS (status))
  {
    printf ("%d: %s\n", format_count, formats[format_count].identifier);
  }
    else
  {
    break;
  }
  }

  if (format_count == 0)
  {
    // no video formats
    return;
  }

  while ((f < 0) || (f >= format_count))
  {
    printf ("Use Format: ");
    //scanf ("%d", &f);
    //TODO get from config
    f=0;
  }

  if (formats[f].size_count)
  {
    int i;
    int s = -1;

    for (i = 0; i < formats[f].size_count; i++)
  {
    printf ("%d: %dx%d\n", i, formats[f].sizes[i].width,
      formats[f].sizes[i].height);
  }

    while ((s < 0) || (s >= formats[f].size_count))
  {
    printf ("Select Size: ");
    //scanf ("%d", &s);
    //TODO get from config
    s=1;
  }

    formats[f].size.width = formats[f].sizes[s].width;
    formats[f].size.height = formats[f].sizes[s].height;
  }

  if (!SUCCESS (unicap_set_format (m_Handle, &formats[f]))) // (3)
  {
    fprintf (stderr, "Failed to set the format!\n");
    exit (-1);
  }
}

size_t ImagingSourceVideoImager::uyvy2rgb24(
                 __u8 *dest,
                 __u8 *source,
                 size_t dest_size,
                 size_t source_size )
{
  unsigned int i;
  int dest_offset = 0;

  if( dest_size < ( source_size + ( source_size >> 1 ) ) )
  {
    printf( "dest_size : %d, source_size: %d !!! ( FAIL ) \n",
        dest_size, source_size );
    return 0;
  }

  //iterate the UYVY stream in block of size 4
  //because in every block there are 2 Y Information (y1,y2) and 1 Cb (u) and 1 Cr(v)
  for( i = 0; i < source_size; i+=4 )
  {
    __u8 *r, *b, *g;
  __u8 *y1, *y2, *u, *v;

  double fr, fg, fb;
  double fy1, fy2, fu, fv;

  r = dest + dest_offset;
  g = r + 1;
  b = g + 1;

  u = source + i;
  y1 = u + 1;
  v = y1 + 1;
  y2 = v + 1;

  fu = *u;
  fv = *v;
  fy1= *y1;
  fy2= *y2;

  fr = fy1 - 0.0009267 * ( fu - 128 ) + 1.4016868 * ( fv - 128 );
  fg = fy1 - 0.3436954 * ( fu - 128 ) - 0.7141690 * ( fv - 128 );
  fb = fy1 + 1.7721604 * ( fu - 128 ) + 0.0009902 * ( fv - 128 );

  *r = (__u8) ( fr > 255 ? 255 : ( fr < 0 ? 0 : fr ) );
  *g = (__u8) ( fg > 255 ? 255 : ( fg < 0 ? 0 : fg ) );
  *b = (__u8) ( fb > 255 ? 255 : ( fb < 0 ? 0 : fb ) );


  dest_offset += 3;

  r = dest + dest_offset;
  g = r + 1;
  b = g + 1;

  fr = fy2 - 0.0009267 * ( fu - 128 ) + 1.4016868 * ( fv - 128 );
  fg = fy2 - 0.3436954 * ( fu - 128 ) - 0.7141690 * ( fv - 128 );
  fb = fy2 + 1.7721604 * ( fu - 128 ) + 0.0009902 * ( fv - 128 );

  *r = (__u8) ( fr > 255 ? 255 : ( fr < 0 ? 0 : fr ) );
  *g = (__u8) ( fg > 255 ? 255 : ( fg < 0 ? 0 : fg ) );
  *b = (__u8) ( fb > 255 ? 255 : ( fb < 0 ? 0 : fb ) );

  dest_offset += 3;
  }

  // From SciLab : This is the good one.
  //r = 1 * y -  0.0009267*(u-128)  + 1.4016868*(v-128);^M
  //g = 1 * y -  0.3436954*(u-128)  - 0.7141690*(v-128);^M
  //b = 1 * y +  1.7721604*(u-128)  + 0.0009902*(v-128);^M

  // YUV->RGB
  // r = 1.164 * (y-16) + 1.596*(v-128);
  // g = 1.164 * (y-16) + 0.813*(v-128) - 0.391*(u-128);
  // b = 1.164 * (y-16) + 2.018*(u-128);
  return source_size * 1.5;
}

} // end of namespace igstk

