/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTrackerNew.cxx
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

#include "igstkFlockOfBirdsTrackerNew.h"


namespace igstk
{

/** Constructor: Initializes all internal variables. */
FlockOfBirdsTracker::FlockOfBirdsTracker(void):m_StateMachine(this)
{

  this->SetThreadingEnabled( true );

  m_BufferLock = itk::MutexLock::New();
  m_Communication = 0;
  m_CommandInterpreter = CommandInterpreterType::New();
}

/** Destructor */
FlockOfBirdsTracker::~FlockOfBirdsTracker(void)
{
}

/** Set the communication object, it will be initialized as necessary
  * for use with the FlockOfBirds */
void FlockOfBirdsTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_CommandInterpreter->SetCommunication( communication );
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker:: Exiting SetCommunication ...\n");
}

/** Open communication with the tracking device. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalOpen called ...\n");
  m_CommandInterpreter->Open();
  m_CommandInterpreter->SetFormat(FB_POSITION_QUATERNION);

  return SUCCESS;
}

/** Close communication with the tracking device. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalClose called ...\n");

  m_CommandInterpreter->Close();

  return SUCCESS;
}

/** Activate the tools attached to the tracking device. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, 
               "FlockOfBirdsTracker::InternalActivateTools called ...\n");
  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalDeactivateTools( void )
{
  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, 
                "FlockOfBirdsTracker::InternalStartTracking called ...\n");
  return SUCCESS;
}

/** Take the tracking device out of tracking mode. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG,
                "FlockOfBirdsTracker::InternalStopTracking called ...\n");
  return SUCCESS;
}

/** Reset the tracking device to put it back to its original state. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalReset( void )
{
  return SUCCESS;
}


/** Update the status and the transforms for all TrackerTools. */
FlockOfBirdsTracker::ResultType FlockOfBirdsTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, 
                 "FlockOfBirdsTracker::InternalUpdateStatus called ...\n");

  // This method and the InternalThreadedUpdateStatus are both called
  // continuously in the Tracking state.  This method is called from
  // the main thread, while InternalThreadedUpdateStatus is called
  // from the thread that actually communicates with the device.
  // A shared memory buffer is used to transfer information between
  // the threads, and it must be locked when either thread is
  // accessing it.
  m_BufferLock->Lock();

  typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;

  InputConstIterator inputItr = this->m_ToolTransformBuffer.begin();
  InputConstIterator inputEnd = this->m_ToolTransformBuffer.end();

  TrackerToolsContainerType trackerToolContainer =
      this->GetTrackerToolContainer();

  while( inputItr != inputEnd )
  {
      // only report tools that are in view
      if (! this->m_ToolStatusContainer[inputItr->first])
      {
          igstkLogMacro( INFO, "igstk::FlockOfBirdTracker::InternalUpdateStatus: " <<
              "tool " << inputItr->first << " is not in view\n");

          // report to the tracker tool that the tracker is not available
          this->ReportTrackingToolNotAvailable(
              trackerToolContainer[inputItr->first]);

          ++inputItr;
          continue;
      }
      // report to the tracker tool that the tracker is Visible
      this->ReportTrackingToolVisible(trackerToolContainer[inputItr->first]);

      // create the transform
      TransformType transform;

      typedef TransformType::VectorType TranslationType;
      TranslationType translation;

      translation[0] = (inputItr->second)[0];
      translation[1] = (inputItr->second)[1];
      translation[2] = (inputItr->second)[2];

      typedef TransformType::VersorType RotationType;
      RotationType rotation;

      const double normsquared =
          (inputItr->second)[3]*(inputItr->second)[3] +
          (inputItr->second)[4]*(inputItr->second)[4] +
          (inputItr->second)[5]*(inputItr->second)[5] +
          (inputItr->second)[6]*(inputItr->second)[6];

      // don't allow null quaternions
      if (normsquared < 1e-6)
      {
          rotation.Set(0.0, 0.0, 0.0, 1.0);
          igstkLogMacro( WARNING, "igstk::FlockOfBirdsTracker::InternUpdateStatus: bad "
              "quaternion, norm=" << sqrt(normsquared) << "\n");
      }
      else
      {
          rotation.Set((inputItr->second)[3],
                       (inputItr->second)[4],
                       (inputItr->second)[5],
                       (inputItr->second)[6]);
      }

      // report error value
      // Get error value from the tracker. TODO
      typedef TransformType::ErrorType  ErrorType;
      ErrorType errorValue = 0.0;

      transform.SetToIdentity(this->GetValidityTime());
      transform.SetTranslationAndRotation(translation, rotation, errorValue,
          this->GetValidityTime());

      // set the raw transform
      this->SetTrackerToolRawTransform(
          trackerToolContainer[inputItr->first], transform );

      this->SetTrackerToolTransformUpdate(
          trackerToolContainer[inputItr->first], true );

      ++inputItr;
  }

  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
FlockOfBirdsTracker::ResultType 
FlockOfBirdsTracker::InternalThreadedUpdateStatus( void )
{

    igstkLogMacro( DEBUG, "FlockOfBirdsTracker::InternalThreadedUpdateStatus "
          "called ...\n");

    // Lock the buffer that this method shares with InternalUpdateStatus
    m_BufferLock->Lock();

    //reset the status of all the tracker tools
    typedef TrackerToolTransformContainerType::const_iterator  InputConstIterator;
    InputConstIterator inputItr = this->m_ToolTransformBuffer.begin();
    InputConstIterator inputEnd = this->m_ToolTransformBuffer.end();

    while( inputItr != inputEnd )
    {
        this->m_ToolStatusContainer[inputItr->first] = 0;
        ++inputItr;
    }

    m_CommandInterpreter->Point();
    m_CommandInterpreter->Update();

    float offset[3];
    m_CommandInterpreter->GetPosition(offset);

    float quaternion[4];
    m_CommandInterpreter->GetQuaternion(quaternion);

     std::vector< double > transform;

     transform.push_back( quaternion[0] ); 
     transform.push_back( quaternion[1] );
     transform.push_back( quaternion[2] );
     transform.push_back( quaternion[3] );
     transform.push_back( offset[0] );
     transform.push_back( offset[1] );
     transform.push_back( offset[2] );

    inputItr = this->m_ToolTransformBuffer.begin();

    if( inputItr != this->m_ToolTransformBuffer.end() )
    {
        this->m_ToolTransformBuffer[inputItr->first] = transform;
        this->m_ToolStatusContainer[inputItr->first] = 1;
    }

    m_BufferLock->Unlock();

    return SUCCESS;
}

/** Enable all tool ports that are occupied. */
void FlockOfBirdsTracker::EnableToolPorts()
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::EnableToolPorts called...\n");

//   for (unsigned int port = 0; port < NumberOfPorts; port++)
//     {
//     this->m_PortEnabled[port] = 1;
//     }

}

/** Disable all tool ports. */
void FlockOfBirdsTracker::DisableToolPorts( void )
{
  igstkLogMacro( DEBUG, "FlockOfBirdsTracker::DisableToolPorts called...\n");
}

/** Verify tracker tool information */
FlockOfBirdsTracker::ResultType
FlockOfBirdsTracker
::VerifyTrackerToolInformation( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, 
    "FlockOfBirdsTracker::VerifyTrackerToolInformation called...\n");
  return SUCCESS;
}

/** Remove tracker tool from internal containers */
FlockOfBirdsTracker::ResultType
FlockOfBirdsTracker
::RemoveTrackerToolFromInternalDataContainers( const TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, 
    "FlockOfBirdsTracker::RemoveTrackerToolFromInternalDataContainers"
    " called...\n");

   const std::string trackerToolIdentifier =
       trackerTool->GetTrackerToolIdentifier();

  std::vector< double > transform;
  transform.push_back( 1.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );
  transform.push_back( 0.0 );

  // remove the tool from the Transform buffer container
  this->m_ToolTransformBuffer.erase( trackerToolIdentifier );
  this->m_ToolStatusContainer.erase( trackerToolIdentifier );

  return SUCCESS;
}

FlockOfBirdsTracker::ResultType
FlockOfBirdsTracker::
AddTrackerToolToInternalDataContainers( const TrackerToolType * trackerTool )
{
    igstkLogMacro( DEBUG,
        "igstk::FlockOfBirdsTracker::AddTrackerToolToInternalDataContainers "
        "called ...\n");

    if ( trackerTool == NULL )
    {
        return FAILURE;
    }

    const std::string trackerToolIdentifier =
        trackerTool->GetTrackerToolIdentifier();

    std::vector< double > transform;
    transform.push_back( 1.0 );
    transform.push_back( 0.0 );
    transform.push_back( 0.0 );
    transform.push_back( 0.0 );
    transform.push_back( 0.0 );
    transform.push_back( 0.0 );
    transform.push_back( 0.0 );

    this->m_ToolTransformBuffer[ trackerToolIdentifier ] = transform;
    this->m_ToolStatusContainer[ trackerToolIdentifier ] = 0;

    return SUCCESS;
}

/** Print Self function */
void FlockOfBirdsTracker::PrintSelf( std::ostream& os, 
                                     itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end of namespace igstk
