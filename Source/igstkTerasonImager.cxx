/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTerasonImager.cxx
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

#include <math.h>

#include "igstkTerasonImager.h"

#include "igstkEvents.h"

#include "vtkImageData.h"

#include <itksys/SystemTools.hxx>

#include <sstream>

namespace igstk
{

//Initialize static variables
std::map< unsigned int, std::string> TerasonImager::m_ErrorCodeContainer;
bool TerasonImager::m_ErrorCodeListCreated = false;

/** Constructor: Initializes all internal variables. */
TerasonImager::TerasonImager(void):m_StateMachine(this)
{
  this->m_NumberOfTools = 0;
  
  this->m_FirstFrame = true;

  this->SetThreadingEnabled( true );

  // Lock for the data buffer that is used to transfer the
  // transformations from thread that is communicating
  // with the imager to the main thread.
  m_BufferLock = itk::MutexLock::New();

  // Create error code list if it hasn't been
  // created.
  //
  if ( ! m_ErrorCodeListCreated )
    {
    this->CreateErrorCodeList();
    m_ErrorCodeListCreated = true;
    }
}

/** Destructor */
TerasonImager::~TerasonImager(void)
{

}

/** Create a map data structure containing MTC error code and description */
void TerasonImager::CreateErrorCodeList()
{
  ErrorCodeContainerType & ecc = m_ErrorCodeContainer;

  ecc[0]  = "OK";
  ecc[1]  = "Invalid object handle";
  ecc[2]  = "Reentrant access - library is not thread-safe";
  ecc[3]  = "Internal TerasonImager software error";
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
TerasonImager::GetErrorDescription( unsigned int code )
{
  if ( code >= 0 && code <= 55 )
    {
    return TerasonImager::m_ErrorCodeContainer[code];
    }
  else
    {
    return "Unknown error code";
    }
}

TerasonImager::ResultType TerasonImager::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::TerasonImager::InternalOpen called ...\n");


  if( ! this->Initialize() )
    {
    igstkLogMacro( CRITICAL, "Error initializing");
    return FAILURE;
    }

  return SUCCESS;
}

/** Initialize socket */
bool TerasonImager::Initialize( void )
{
  igstkLogMacro( DEBUG, "igstk::TerasonImager::Initialize called ...\n");

  bool result = true;

  return result;
}

/** Set the communication object*/
void TerasonImager::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, 
    "igstk::TerasonImager:: Entered SetCommunication ...\n");
  m_Communication = communication;
}

/** Verify imager tool information. */
TerasonImager::ResultType
TerasonImager
::VerifyImagerToolInformation( const ImagerToolType * imagerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::TerasonImager::VerifyImagerToolInformation called ...\n");

  return SUCCESS;
}

/** Detach camera. */
TerasonImager::ResultType TerasonImager::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::TerasonImager::InternalClose called ...\n");
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
TerasonImager::ResultType TerasonImager::InternalStartImaging( void )
{
  igstkLogMacro( DEBUG,
    "igstk::TerasonImager::InternalStartImaging called ...\n");

  // Report errors, if any, and return SUCCESS or FAILURE
  // (the return value will be used by the superclass to
  //  set the appropriate input to the state machine)
  //

  if ( m_Communication.IsNull() )
  {
    std::cout << " m_Communication is NULL. cannot start " << std::endl;
    return FAILURE;
  }

  m_Communication->Print(std::cout);

  // Waiting for Connection
  m_Socket = NULL;
  
  unsigned int times = 0;
  unsigned int maxTimes = 20;

  do 
  {
    times++;

    igstk::DoubleTypeEvent evt;
    evt.Set( (double)times/(double)maxTimes );
    this->InvokeEvent( evt );

    m_Socket = m_Communication->WaitForConnection(1000);
  } while ( (times<maxTimes) && ( m_Socket.IsNull() ) );

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
  }

  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
TerasonImager::ResultType TerasonImager::InternalStopImaging( void )
{
  igstkLogMacro( DEBUG,
    "igstk::TerasonImager::InternalStopImaging called ...\n");

  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
TerasonImager::ResultType TerasonImager::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::TerasonImager::InternalReset called ...\n");
  return SUCCESS;
}


/** Update the status and the transforms for all ImagerTools. */
TerasonImager::ResultType TerasonImager::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG,
    "igstk::TerasonImager::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Imaging state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  typedef ImagerToolFrameContainerType::const_iterator  InputConstIterator;

  InputConstIterator inputItr = this->m_ToolFrameBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolFrameBuffer.end();

  ImagerToolsContainerType imagerToolContainer =
    this->GetImagerToolContainer();

  unsigned int toolId = 0;

  while( inputItr != inputEnd )
    {      
    // only report tools that have useful data
    if (! this->m_ToolStatusContainer[inputItr->first])
    {
      igstkLogMacro( DEBUG, "igstk::TerasonImager::InternalUpdateStatus: " <<
              "tool " << inputItr->first << " is not in view\n");
      // report to the imager tool that the imager is not available
      this->ReportImagingToolNotAvailable(
        imagerToolContainer[inputItr->first]);

      ++inputItr;
      continue;
    }
        
    // report to the imager tool that the tool is Visible
    this->ReportImagingToolVisible(imagerToolContainer[inputItr->first]);

    this->SetImagerToolFrame(
      imagerToolContainer[inputItr->first], (inputItr->second) );

    this->SetImagerToolUpdate(
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
TerasonImager::ResultType 
TerasonImager::InternalThreadedUpdateStatus( void )
{
    igstkLogMacro( DEBUG, "igstk::TerasonImager::InternalThreadedUpdateStatus "
                 "called ...\n");

    igstkLogMacro( DEBUG, "InternalThreadedUpdateStatus entered" );

    // Lock the buffer that this method shares with InternalUpdateStatus
    m_BufferLock->Lock();

    //reset the status of all the tracker tools
    typedef ImagerToolFrameContainerType::const_iterator  InputConstIterator;
    InputConstIterator inputItr = this->m_ToolFrameBuffer.begin();
    InputConstIterator inputEnd = this->m_ToolFrameBuffer.end();

    while( inputItr != inputEnd )
    {
      this->m_ToolStatusContainer[inputItr->first] = 0;
      ++inputItr;
    }

    try
    {      

      m_HeaderMsg = igtl::MessageHeader::New();

      // Initialize receive buffer
      m_HeaderMsg->InitPack();

      // Receive generic header from the socket
      int r = 0;
    
      r = this->m_Socket->Receive(m_HeaderMsg->GetPackPointer(), m_HeaderMsg->GetPackSize());

      if (r != m_HeaderMsg->GetPackSize())
      { 
        igstkLogMacro( CRITICAL, "Error in pack size" );
        m_BufferLock->Unlock();
        return FAILURE;        
      }

      igstkLogMacro( DEBUG, "InternalThreadedUpdateStatus Receive passed" );
      
      // Deserialize the header
      m_HeaderMsg->Unpack();
         
      // Check data type and receive data body
      if (strcmp(m_HeaderMsg->GetDeviceType(), "IMAGE") == 0)
      {     
          // Create a message buffer to receive transform data
          igtl::ImageMessage::Pointer imgMsg;          
          imgMsg = igtl::ImageMessage::New();            
          
         // if (m_FirstFrame)
         // {
            imgMsg->SetMessageHeader(m_HeaderMsg);
            imgMsg->AllocatePack();
            m_FirstFrame = false;
         // }

          // Receive transform data from the socket
          this->m_Socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());
          
          // Deserialize the transform data
          // If you want to skip CRC check, call Unpack() without argument.
          int c = imgMsg->Unpack(1);
          
          if (!(c & igtl::MessageHeader::UNPACK_BODY)) // if CRC check is OK
          {
            igstkLogMacro( CRITICAL, "Error in CRC check while unpacking" );
            m_BufferLock->Unlock();
            return FAILURE;
          }
           
          // Check if an imager tool was added with this device name        
          typedef ImagerToolFrameContainerType::iterator InputIterator;

          InputIterator deviceItr =
            this->m_ToolFrameBuffer.find( imgMsg->GetDeviceName() );

          if( deviceItr != this->m_ToolFrameBuffer.end() )
          {
            // Retrive the incoming image dimensions
            //unsigned int   inDims[3];
            //imgMsg->GetDimensions(inDims);
            
            int fsize = imgMsg->GetImageSize();
              
            // create the frame
            ImagerToolsContainerType imagerToolContainer =
              this->GetImagerToolContainer();

            unsigned int toolDims[3];          // image dimension set on tools
            imagerToolContainer[deviceItr->first]->GetFrameDimensions(toolDims);
           
            int toolSize = toolDims[0] * toolDims[1] * toolDims[2];

            if (fsize != toolSize)
            {
              igstkLogMacro( CRITICAL, "Incoming image size does not match with expected" );
              m_BufferLock->Unlock();
              return FAILURE;
            }

            FrameType frame;
            this->GetImagerToolFrame( imagerToolContainer[deviceItr->first], frame );
               
            memcpy(frame.GetImagePtr(),imgMsg->GetScalarPointer(),fsize);            

            this->m_ToolFrameBuffer[ deviceItr->first ] = frame;
            this->m_ToolStatusContainer[ deviceItr->first ] = 1;
          }

          m_BufferLock->Unlock();
          return SUCCESS;
      }
      else
      {
        this->m_Communication->Skip(m_HeaderMsg->GetBodySizeToRead(), 0);       
        m_BufferLock->Unlock();
        return SUCCESS;
      }
  }
  catch(...)
  {
      igstkLogMacro( CRITICAL, "Unknown error catched" );      
      m_BufferLock->Unlock();
      return FAILURE;  
  }
}

TerasonImager::ResultType
TerasonImager::
AddImagerToolToInternalDataContainers( const ImagerToolType * imagerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::TerasonImager::RemoveImagerToolFromInternalDataContainers "
                 "called ...\n");

  if ( imagerTool == NULL )
    {
    return FAILURE;
    }

  const std::string imagerToolIdentifier =
                    imagerTool->GetImagerToolIdentifier();

 // igtl::ImageMessage::Pointer imgMsg;
  igstk::Frame frame;

  this->m_ToolFrameBuffer[ imagerToolIdentifier ] = frame;
  this->m_ToolStatusContainer[ imagerToolIdentifier ] = 0;

  return SUCCESS;
}


TerasonImager::ResultType
TerasonImager::
RemoveImagerToolFromInternalDataContainers
( const ImagerToolType * imagerTool )
{
  igstkLogMacro( DEBUG,
    "igstk::TerasonImager::RemoveImagerToolFromInternalDataContainers "
                 "called ...\n");

  const std::string imagerToolIdentifier =
                      imagerTool->GetImagerToolIdentifier();

  // remove the tool from the frame buffer container
  //this->m_ToolTransformBuffer.erase( imagerToolIdentifier );
  this->m_ToolStatusContainer.erase( imagerToolIdentifier );

  this->m_ToolFrameBuffer.erase( imagerToolIdentifier );

  return SUCCESS;
}

/**The "ValidateSpecifiedFrequency" method checks if the specified
  * frequency is valid for the tracking device that is being used. */
TerasonImager::ResultType
TerasonImager::ValidateSpecifiedFrequency( double frequencyInHz )
{
  const double MAXIMUM_FREQUENCY = 30;
  if ( frequencyInHz < 0.0 || frequencyInHz > MAXIMUM_FREQUENCY )
    {
    return FAILURE;
    } 
  return SUCCESS;
}


/** Print Self function */
void TerasonImager::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << " output Open IGTLink parameters " << std::endl;
}

} // end of namespace igstk
