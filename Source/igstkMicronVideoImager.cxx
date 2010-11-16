/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronVideoImager.cxx
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


//
// These header files can be found in the Utilities/MicronTracker subdirectory.
//
#include "Markers.h"
#include "Marker.h"
#include "Persistence.h"
#include "Cameras.h"
#include "MCamera.h"
#include "Facet.h"
#include "Xform3D.h"
#include "MTC.h"

#include <sstream>


#include "igstkMicronVideoImager.h"
#include "igstkEvents.h"

#include <itksys/SystemTools.hxx>

namespace igstk
{


  /** Constructor: Initializes all internal variables. */
  MicronVideoImager::MicronVideoImager(void):m_StateMachine(this)
  {
    this->m_NumberOfTools = 0;

    this->SetThreadingEnabled( true );

    // Lock for the data buffer that is used to transfer the
    // frames from thread that is communicating
    // with the imager to the main thread.
    //m_BufferLock = itk::MutexLock::New();

    m_FrameBufferLock = itk::MutexLock::New();

    this->m_VideoWidth = this->m_VideoHeight = 0;

  }

  /** Destructor */
  MicronVideoImager::~MicronVideoImager(void)
  {

  }

  MicronVideoImager::ResultType
  MicronVideoImager::InternalOpen( void )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::InternalOpen called ...\n");

    if( m_MicronTracker.IsNull() )
    {
      igstkLogMacro( CRITICAL, "Associated MicronTracker is null");
      return FAILURE;
    }
    return SUCCESS;
  }


  bool MicronVideoImager::SetUpCameras( void )
  {
    igstkLogMacro( DEBUG, "igstk::MicronVideoImager::SetUpCameras called ...\n");

    bool result = true;
    if ( m_MicronTracker->m_Cameras->getCount() < 1)
    {
      igstkLogMacro(CRITICAL,"No camera connection available in MicronTracker ");
      result = false;
    }
    else
    {
      this->m_VideoWidth = m_MicronTracker->m_SelectedCamera->getXRes();
      this->m_VideoHeight = m_MicronTracker->m_SelectedCamera->getYRes();
    }
    return result;
  }

  /** Verify imager tool information. */
  MicronVideoImager::ResultType
  MicronVideoImager
  ::VerifyVideoImagerToolInformation( const VideoImagerToolType * imagerTool )
  {
    igstkLogMacro( DEBUG, "igstk::MicronVideoImager"
        << "::VerifyVideoImagerToolInformation called ...\n");

    return SUCCESS;
  }

  /** Detach camera. */
  MicronVideoImager::ResultType
  MicronVideoImager::InternalClose( void )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::InternalClose called ...\n");

    return SUCCESS;
  }

  /** Put the imaging device into imaging mode. */
  MicronVideoImager::ResultType
  MicronVideoImager::InternalStartImaging( void )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::InternalStartImaging called ...\n");
    // Report errors, if any, and return SUCCESS or FAILURE
    // (the return value will be used by the superclass to
    //  set the appropriate input to the state machine)
    //

    if( ! this->SetUpCameras() )
    {
      igstkLogMacro( CRITICAL, "Error setting up cameras ");
      return FAILURE;
    }

    return SUCCESS;
  }

  /** Take the imaging device out of imaging mode. */
  MicronVideoImager::ResultType
  MicronVideoImager::InternalStopImaging( void )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::InternalStopImaging called ...\n");

    // Do not detach camera. It is the MicronTracker
    // Responsibility to detach it

    return SUCCESS;
  }

  /** Reset the imaging device to put it back to its original state. */
  MicronVideoImager::ResultType
  MicronVideoImager::InternalReset( void )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::InternalReset called ...\n");

    // Do not detach camera. It is the MicronTracker
    // Responsibility to detach it

    return SUCCESS;
  }

  /** Update the status and the transforms for all VideoImagerTools. */
  MicronVideoImager::ResultType
  MicronVideoImager::InternalUpdateStatus()
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::InternalUpdateStatus called ...\n");

    // This method and the InternalThreadedUpdateStatus are both called
    // continuously in the Imaging state.  This method is called from
    // the main thread, while InternalThreadedUpdateStatus is called
    // from the thread that actually communicates with the device.
    // A shared memory buffer is used to transfer information between
    // the threads, and it must be locked when either thread is
    // accessing it.
    m_MicronTracker->m_BufferLock->Lock();

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
        igstkLogMacro( DEBUG, "igstk::MicronVideoImager"
            << "::InternalUpdateStatus: " <<
            "tool " << inputItr->first << " is not in view\n");
        // report to the imager tool that the imager is not available
        this->ReportImagingToolNotAvailable(
            imagerToolContainer[inputItr->first]);

        ++inputItr;
        continue;
      }

      // report to the imager tool that the tool is sending frames
      this->ReportImagingToolStreaming(imagerToolContainer[inputItr->first]);

      this->SetVideoImagerToolFrame(
          imagerToolContainer[inputItr->first], (inputItr->second) );

      this->SetVideoImagerToolUpdate(
          imagerToolContainer[inputItr->first], true );

      ++inputItr;
      ++toolId;
    }

    m_MicronTracker->m_BufferLock->Unlock();

    return SUCCESS;
  }

  /** Update the shared memory buffer and the tool's internal frame. This function
  *  is called by the thread that communicates with the imager while
  *  the imager is in the Imaging state. */
  MicronVideoImager::ResultType
  MicronVideoImager::InternalThreadedUpdateStatus( void )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::InternalThreadedUpdateStatus called ...\n");
    // Lock the buffer that this method shares with InternalUpdateStatus
    m_MicronTracker->m_BufferLock->Lock();

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
      igstkLogMacro( DEBUG, "InternalThreadedUpdateStatus Receive passed\n" );

      unsigned char **m_LeftFrameBuffer, **m_RightFrameBuffer;
      m_MicronTracker->m_SelectedCamera->getImages(&m_LeftFrameBuffer, &m_RightFrameBuffer);

      // Check if an imager tool was added with this device name
      typedef VideoImagerToolFrameContainerType::iterator InputIterator;

      //TODO toolname hard coded
      /**
      * Get Left image
      */
      InputIterator deviceItr = this->m_ToolFrameBuffer.find( "Left" );

      if( deviceItr != this->m_ToolFrameBuffer.end() )
      {
        // create the frame
        VideoImagerToolsContainerType imagerToolContainer =
          this->GetVideoImagerToolContainer();
        FrameType* frame = new FrameType();
        frame = this->GetVideoImagerToolFrame(
            imagerToolContainer[deviceItr->first]);

        unsigned int frameDims[3];
        imagerToolContainer[deviceItr->first]->GetFrameDimensions(frameDims);

        this->m_FrameBufferLock->Lock();

        if( m_LeftFrameBuffer !=NULL)
        {

          memcpy(frame->GetImagePtr(),
              (unsigned char*)m_LeftFrameBuffer,
              frameDims[0]*frameDims[1]*frameDims[2]);
        }

        this->m_FrameBufferLock->Unlock();

        //update frame validity time
        frame->SetTimeToExpiration(this->GetValidityTime());

        this->m_ToolFrameBuffer[ deviceItr->first ] = frame;
        this->m_ToolStatusContainer[ deviceItr->first ] = 1;
      }

      /**
      * Get right image
      */
      deviceItr = this->m_ToolFrameBuffer.find( "Right" );

      if( deviceItr != this->m_ToolFrameBuffer.end() )
      {
        // create the frame
        VideoImagerToolsContainerType imagerToolContainer =
          this->GetVideoImagerToolContainer();
        FrameType* frame = new FrameType();
        frame = this->GetVideoImagerToolFrame(
            imagerToolContainer[deviceItr->first]);

        unsigned int frameDims[3];
        imagerToolContainer[deviceItr->first]->GetFrameDimensions(frameDims);

        this->m_FrameBufferLock->Lock();

        if( m_RightFrameBuffer !=NULL)
        {
          memcpy(frame->GetImagePtr(),
              (unsigned char*)m_RightFrameBuffer,
              frameDims[0]*frameDims[1]*frameDims[2]);
        }

        this->m_FrameBufferLock->Unlock();

        //update frame validity time
        frame->SetTimeToExpiration(this->GetValidityTime());

        this->m_ToolFrameBuffer[ deviceItr->first ] = frame;
        this->m_ToolStatusContainer[ deviceItr->first ] = 1;
      }
      m_MicronTracker->m_BufferLock->Unlock();
      return SUCCESS;
    }
    catch(...)
    {
      igstkLogMacro( CRITICAL, "Unknown error catched" );
      m_MicronTracker->m_BufferLock->Unlock();
      return FAILURE;
    }
  }

  MicronVideoImager::ResultType
  MicronVideoImager::
  AddVideoImagerToolToInternalDataContainers( const VideoImagerToolType * imagerTool )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::RemoveVideoImagerToolFromInternalDataContainers "
        "called ...\n");

    if ( imagerTool == NULL )
    {
      return FAILURE;
    }

    const std::string imagerToolIdentifier =
      imagerTool->GetVideoImagerToolIdentifier();

    igstk::Frame* frame = new igstk::Frame;

    this->m_ToolFrameBuffer[ imagerToolIdentifier ] = frame;
    this->m_ToolStatusContainer[ imagerToolIdentifier ] = 0;

    return SUCCESS;
  }


  MicronVideoImager::ResultType
  MicronVideoImager::
  RemoveVideoImagerToolFromInternalDataContainers
  ( const VideoImagerToolType * imagerTool )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImager::RemoveVideoImagerToolFromInternalDataContainers "
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
  MicronVideoImager::ResultType
  MicronVideoImager::ValidateSpecifiedFrequency( double frequencyInHz )
  {
    const double MAXIMUM_FREQUENCY = 30;
    if ( frequencyInHz < 0.0 || frequencyInHz > MAXIMUM_FREQUENCY )
    {
      return FAILURE;
    }
    return SUCCESS;
  }


  /** Print Self function */
  void MicronVideoImager::PrintSelf( std::ostream& os, itk::Indent indent ) const
  {
    Superclass::PrintSelf(os, indent);

    os << indent << " output Imaging Source Converter parameters " << std::endl;
  }

} // end of namespace igstk
