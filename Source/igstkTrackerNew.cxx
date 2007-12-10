/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerNew.cxx
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
//Warning about: identifier was truncated to '255' characters in the debug 
// information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkTrackerNew.h"

namespace igstk
{

/** Constructor */
TrackerNew::TrackerNew(void) :  m_StateMachine( this ) 
{
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  // Set the state descriptors
  igstkAddStateMacro( Idle ); 
  igstkAddStateMacro( AttemptingToEstablishCommunication ); 
  igstkAddStateMacro( AttemptingToCloseCommunication); 
  igstkAddStateMacro( AttemptingToAttachTrackerTool );
  igstkAddStateMacro( TrackerToolAttached );
  igstkAddStateMacro( CommunicationEstablished );
  igstkAddStateMacro( AttemptingToTrack ); 
  igstkAddStateMacro( AttemptingToStopTracking); 
  igstkAddStateMacro( Tracking ); 
  igstkAddStateMacro( AttemptingToUpdate ); 
  
  // Set the input descriptors
  igstkAddInputMacro( EstablishCommunication);
  igstkAddInputMacro( AttachTrackerTool);
  igstkAddInputMacro( StartTracking); 
  igstkAddInputMacro( UpdateStatus); 
  igstkAddInputMacro( StopTracking); 
  igstkAddInputMacro( Reset); 
  igstkAddInputMacro( CloseCommunication); 
  igstkAddInputMacro( Success); 
  igstkAddInputMacro( Failure); 

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           EstablishCommunication,
                           AttemptingToEstablishCommunication,
                           AttemptToOpen );
  igstkAddTransitionMacro( Idle,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           AttachTrackerTool,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           UpdateStatus,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Reset,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );

  // Transitions from the AttemptingToEstablishCommunication
  igstkAddTransitionMacro( AttemptingToEstablishCommunication,
                           Success,
                           CommunicationEstablished,
                           CommunicationEstablishmentSuccess );

  igstkAddTransitionMacro( AttemptingToEstablishCommunication,
                           Failure,
                           Idle,
                           CommunicationEstablishmentFailure );

  // Transitions from CommunicationEstablished
  igstkAddTransitionMacro( CommunicationEstablished,
                           AttachTrackerTool,
                           AttemptingToAttachTrackerTool,
                           AttemptToAttachTrackerTool );

  igstkAddTransitionMacro( CommunicationEstablished,
                           StartTracking,
                           AttemptingToTrack,
                           AttemptToStartTracking );

  igstkAddTransitionMacro( CommunicationEstablished,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromCommunicatingState );

  igstkAddTransitionMacro( CommunicationEstablished,
                           Reset,
                           CommunicationEstablished,
                           ResetFromCommunicatingState );

  igstkAddTransitionMacro( CommunicationEstablished,
                           StopTracking,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( CommunicationEstablished,
                           EstablishCommunication,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( CommunicationEstablished,
                           UpdateStatus,
                           CommunicationEstablished,
                           ReportInvalidRequest );

  // Transitions from AttemptingToAttachTrackerTool
  igstkAddTransitionMacro( AttemptingToAttachTrackerTool,
                           Success,
                           TrackerToolAttached,
                           AttachingTrackerToolSuccess );

  igstkAddTransitionMacro( AttemptingToAttachTrackerTool,
                           Failure,
                           CommunicationEstablished,
                           AttachingTrackerToolFailure );

  // Transitions from TrackerToolAttached
  igstkAddTransitionMacro( TrackerToolAttached,
                           StartTracking,
                           AttemptingToTrack,
                           AttemptToStartTracking );

  igstkAddTransitionMacro( TrackerToolAttached,
                           AttachTrackerTool,
                           AttemptingToAttachTrackerTool,
                           AttemptToAttachTrackerTool );


  // Transitions from AttemptingToTrack
  igstkAddTransitionMacro( AttemptingToTrack,
                           Success,
                           Tracking,
                           StartTrackingSuccess );

  igstkAddTransitionMacro( AttemptingToTrack,
                           Failure,
                           CommunicationEstablished,
                           StartTrackingFailure );

  // Transitions from Tracking
  igstkAddTransitionMacro( Tracking,
                           UpdateStatus,
                           AttemptingToUpdate,
                           AttemptToUpdateStatus );

  igstkAddTransitionMacro( Tracking,
                           StopTracking,
                           AttemptingToStopTracking,
                           AttemptToStopTracking );

  igstkAddTransitionMacro( Tracking,
                           Reset,
                           CommunicationEstablished,
                           ResetFromTrackingState );

  igstkAddTransitionMacro( Tracking,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromTrackingState );

  // Transitions from AttemptingToUpdate
  igstkAddTransitionMacro( AttemptingToUpdate,
                           Success,
                           Tracking,
                           UpdateStatusSuccess );

  igstkAddTransitionMacro( AttemptingToUpdate,
                           Failure,
                           Tracking,
                           UpdateStatusFailure );

  // Transitions from AttemptingToStopTracking
  igstkAddTransitionMacro( AttemptingToStopTracking,
                           Success,
                           CommunicationEstablished,
                           StopTrackingSuccess );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           Failure,
                           Tracking,
                           StopTrackingFailure );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Success,
                           Idle,
                           CloseCommunicationSuccess );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Failure,
                           CommunicationEstablished,
                           CloseCommunicationFailure );

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Create a PulseGenerator object.  
  m_PulseGenerator = PulseGenerator::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & TrackerNew::RequestUpdateStatus );
  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  // This is update rate for sending tracking information to the
  // spatial objects, it should be set to at least 30 Hz
  m_PulseGenerator->RequestSetFrequency( 30 );

  // This is the time period for which transformation should be
  // considered valid.  After this time, they expire.  This time
  // is in milliseconds.
  m_ValidityTime = 400.0;

  // By default, the reference is not used
  m_ApplyingReferenceTool = false;

  m_ConditionNextTransformReceived = itk::ConditionVariable::New();
  m_Threader = itk::MultiThreader::New();
  m_ThreadingEnabled = false;

}

/** Destructor */
TrackerNew::~TrackerNew(void)
{
}

/** This method sets the reference tool. */
void TrackerNew::RequestSetReferenceTool( TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::RequestStartTracking called ...\n");
  // connect the reference tracker tool the tracker 
  TransformType identityTransform;
  identityTransform.SetToIdentity( 
                    igstk::TimeStamp::GetLongestPossibleTime() );
  
  trackerTool->RequestSetTransformAndParent( identityTransform, this );

  if( trackerTool != NULL )
    {
    // check if it is already attached to the tracker
    typedef TrackerToolsContainerType::iterator InputIterator;
    InputIterator toolItr = 
                m_TrackerTools.find( trackerTool->GetTrackerToolIdentifier() );

    if( toolItr != m_TrackerTools.end() )
      {
      m_ApplyingReferenceTool = true;
      m_ReferenceTool = trackerTool;

      // FIXME: Connect the coordinate system of all the other tracker tools to
      //  the reference tracker tool. In other words, make reference tracker tool
      //  the parent of all the other tracker tools.
      }
    else
      {
      std::cerr << "Request to use a tracker tool as a reference has failed."
                << "The tracker tool is not attached to the tracker " << std::endl;
      }
   }
}

/** The "RequestOpen" method attempts to open communication with the
 *  tracking device. */
void TrackerNew::RequestOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::RequestOpen called...\n");
  igstkPushInputMacro( EstablishCommunication );
  this->m_StateMachine.ProcessInputs();
}


/** The "RequestClose" method closes communication with the device. */
void TrackerNew::RequestClose( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::RequestClose called ...\n");
  igstkPushInputMacro( CloseCommunication );
  m_StateMachine.ProcessInputs();
}

/** The "RequestReset" tracker method should be used to bring the tracker
 * to some defined default state. */
void TrackerNew::RequestReset( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::RequestReset called ...\n");
  igstkPushInputMacro( Reset );
  m_StateMachine.ProcessInputs();
}


/** The "RequestStartTracking" method readies the tracker for tracking the
 *  tools connected to the tracker. */
void TrackerNew::RequestStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::RequestStartTracking called ...\n");
  igstkPushInputMacro( StartTracking );
  m_StateMachine.ProcessInputs();
}


/** The "RequestStopTracking" stops tracker from tracking the tools. */
void TrackerNew::RequestStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::RequestStopTracking called ...\n");
  igstkPushInputMacro( StopTracking );
  m_StateMachine.ProcessInputs();
}


/** The "RequestUpdateStatus" method is used for updating the status of 
 *  ports and tools when the tracker is in tracking state. */
void TrackerNew::RequestUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::RequestUpdateStatus called ...\n");
  igstkPushInputMacro( UpdateStatus );
  m_StateMachine.ProcessInputs();
}

/** The "InternalOpen" method opens communication with a tracking device.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalOpen called ...\n");
  return SUCCESS;
}

/** The "InternalClose" method closes communication with a tracking device.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalClose called ...\n");
  return SUCCESS;
}


/** The "InternalReset" method resets tracker to a known configuration. 
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalReset called ...\n");
  return SUCCESS;
}

/** The "InternalStartTracking" method starts tracking.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalStartTracking called ...\n");
  return SUCCESS;
}


/** The "InternalStopTracking" method stops tracking.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalStopTracking called ...\n");
  return SUCCESS;
}


/** The "InternalUpdateStatus" method updates tracker status.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalUpdateStatus called ...\n");
  return SUCCESS;
}


/** The "InternalThreadedUpdateStatus" method updates tracker status.
 *  This method is called in a separate thread.
 *  This method is to be overridden by a descendant class
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerNew::InternalThreadedUpdateStatus called ...\n");
  return SUCCESS;
}


/** The "AttemptToOpen" method attempts to open communication with a
 *  tracking device. */
void TrackerNew::AttemptToOpenProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerNew::AttemptToOpenProcessing called ...\n");

  ResultType result = this->InternalOpen();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after communication setup has been successful. */ 
void TrackerNew::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerNew::CommunicationEstablishmentSuccessProcessing called ...\n");

  this->InvokeEvent( TrackerNewOpenEvent() );
}


/** Post-processing after communication setup has failed. */ 
void TrackerNew::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "CommunicationEstablishmentFailureProcessing called ...\n");

  this->InvokeEvent( TrackerNewOpenErrorEvent() );
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished state */
void TrackerNew::ResetFromTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "ResetFromTrackingStateProcessing() called ...\n");
  // leaving TrackingState, going to CommunicationEstablishedState
  this->ExitTrackingStateProcessing();
  this->ResetFromToolsActiveStateProcessing();
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished  state */
void TrackerNew::ResetFromToolsActiveStateProcessing( void )
{
  igstkLogMacro( DEBUG, 
         "igstk::Tracker::ResetFromToolsActiveStateProcessing() called ...\n");
  this->ResetFromCommunicatingStateProcessing();
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished state */
void TrackerNew::ResetFromCommunicatingStateProcessing( void )
{
  ResultType result = this->InternalReset();

  if( result == SUCCESS )
    {
    igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalReset succeeded ...\n");
    }
  else if( result == FAILURE )
    {
    igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalReset failed ...\n");
    }
}

/** Post-processing after ports and tools setup has been successful. */ 
void TrackerNew::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::ToolsActivationSuccessProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerNewInitializeEvent() );
}

/** Post-processing after ports and tools setup has failed. */ 
void TrackerNew::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::ToolsActivationFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerNewInitializeErrorEvent() );
}

/** The "AttemptToStartTracking" method attempts to start tracking. */
void TrackerNew::AttemptToStartTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::AttemptToStartTrackingProcessing  "
                 "called ...\n");

  ResultType result = this->InternalStartTracking();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Post-processing after start tracking has been successful. */ 
void TrackerNew::StartTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::StartTrackingSuccessProcessing "
                 "called ...\n");

  // Report to all the tracker tools that tracking has been started
  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();
  InputConstIterator inputEnd = m_TrackerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->ReportTrackingStarted();
    ++inputItr;
    }


  // going from AttemptingToTrackState to TrackingState
  this->EnterTrackingStateProcessing();

  this->InvokeEvent( TrackerNewStartTrackingEvent() );
}

/** Post-processing after start tracking has failed. */ 
void TrackerNew::StartTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::StartTrackingFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerNewStartTrackingErrorEvent() );
}

/** Post-processing after attaching a tracker tool to the tracker
 *  has been successful. */ 
void TrackerNew::AttachingTrackerToolSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::AttachingTrackerToolSuccessProcessing "
                 "called ...\n");

  m_TrackerTools[ m_IdentifierForTrackerToolToBeAttached ] 
                                   = m_TrackerToolToBeAttached; 

  // report to the tracker tool that the attachment has been 
  // successful
  m_TrackerToolToBeAttached->ReportSuccessfulTrackerToolAttachment();

  //connect the tracker tool coordinate system to the tracker
  //system. By default, make the tracker coordinate system to 
  //be a parent of the tracker tool coordinate system
  //If a reference tracker tool is specified, the reference
  //tracker tool will become the parent of all the tracker tools.
  TransformType identityTransform;
  identityTransform.SetToIdentity( 
                  igstk::TimeStamp::GetLongestPossibleTime() );

  m_TrackerToolToBeAttached->RequestSetTransformAndParent( identityTransform, this );

  this->InvokeEvent( AttachingTrackerToolToTrackerNewEvent() );
}

/** Post-processing after attaching a tracker tool to the tracker
 *  has failed. */ 
void TrackerNew::AttachingTrackerToolFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::AttachingTrackerToolFailureProcessing "
                 "called ...\n");

  // report to the tracker tool that the attachment has failed
  m_TrackerToolToBeAttached->ReportFailedTrackerToolAttachment();

  this->InvokeEvent( AttachingTrackerToolToTrackerNewErrorEvent() );
}
 
/** The "AttemptToStopTracking" method attempts to stop tracking. */
void TrackerNew::AttemptToStopTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::AttemptToStopTrackingProcessing "
                        "called ...\n");
  // leaving TrackingState, going to AttemptingToStopTrackingState
  this->ExitTrackingStateProcessing();

  ResultType result = this->InternalStopTracking();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after stop tracking has been successful. */ 
void TrackerNew::StopTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::StopTrackingSuccessProcessing "
                 "called ...\n");

  // Report to all the tracker tools that tracking has been stopped
  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();
  InputConstIterator inputEnd = m_TrackerTools.end();

  while( inputItr != inputEnd )
    {
    (inputItr->second)->ReportTrackingStopped();
    ++inputItr;
    }

  this->InvokeEvent( TrackerNewStopTrackingEvent() );  
}

/** Post-processing after start tracking has failed. */ 
void TrackerNew::StopTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::StopTrackingFailureProcessing "
                 "called ...\n");
  // going from AttemptingToStopTrackingState to TrackingState
  this->EnterTrackingStateProcessing();

  this->InvokeEvent( TrackerNewStopTrackingErrorEvent() );  
}

/** Needs to be called every time when entering tracking state. */ 
void TrackerNew::EnterTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::EnterTrackingStateProcessing "
                 "called ...\n");

  if ( this->GetThreadingEnabled() )
    {
    m_ThreadID = m_Threader->SpawnThread( TrackingThreadFunction, this );
    }

  m_PulseGenerator->RequestStart();
}

/** Needs to be called every time when exiting tracking state. */ 
void TrackerNew::ExitTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::ExitTrackingStateProcessing "
                 "called ...\n");
  m_PulseGenerator->RequestStop();

  // Terminating the TrackingThread.
  if ( this->GetThreadingEnabled() )
    {
    m_Threader->TerminateThread( m_ThreadID );
    }
}

/** The "AttemptToUpdateStatus" method attempts to update status
    during tracking. */
void TrackerNew::AttemptToUpdateStatusProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::AttemptToUpdateStatusProcessing "
                        "called ...\n");

  // Set all tools to "not updated"
  //
  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();
  InputConstIterator inputEnd = m_TrackerTools.end();

  unsigned int toolId = 0;

  while( inputItr != inputEnd )
    {
    (inputItr->second)->SetUpdated(false);
    ++inputItr;
    }
 
  // wait for a new transform to be available, it would be nice if
  // "Wait" had a time limit like pthread_cond_timedwait() on Unix or
  // WaitForSingleObject() on Windows
  if ( this->GetThreadingEnabled() )
    {
    m_ConditionNextTransformReceived->Wait( 
      & m_LockForConditionNextTransformReceived );
    }
  else
    {
    this->InternalThreadedUpdateStatus();
    }

  ResultType result = this->InternalUpdateStatus();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** This method is called when a call to UpdateStatus succeeded */
void TrackerNew::UpdateStatusSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::UpdateStatusSuccessProcessing "
                 "called ...\n");

  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();
  InputConstIterator inputEnd = m_TrackerTools.end();

  while( inputItr != inputEnd )
    {
    if ( (inputItr->second)->GetUpdated() &&
           ( !m_ApplyingReferenceTool || m_ReferenceTool->GetUpdated() ) ) 
      {
      TransformType transform = (inputItr->second)->GetRawTransform();

      const double timeToExpiration = transform.GetExpirationTime() - 
                                      transform.GetStartTime();

      TransformType::VersorType rotation;
      TransformType::VectorType translation;

      translation = transform.GetTranslation();
      rotation    = transform.GetRotation();

      TransformType toolRawTransform;
      toolRawTransform.SetTranslationAndRotation( translation, rotation,
                        transform.GetError(),
                        timeToExpiration );

      (inputItr->second)->SetRawTransform( toolRawTransform );
 
      CalibrationTransformType toolCalibrationTransform
                                    = (inputItr->second)->GetCalibrationTransform();

      rotation = toolCalibrationTransform.GetRotation();
      translation = toolCalibrationTransform.GetTranslation();

      // transform by the tracker's tool transform
      rotation = transform.GetRotation()*rotation;
      translation = transform.GetRotation().Transform(translation);
      translation += transform.GetTranslation();

      TransformType toolCalibratedRawTransform;
      toolCalibratedRawTransform.SetTranslationAndRotation( translation, rotation,
                        transform.GetError(),
                        timeToExpiration );


      (inputItr->second)->SetCalibratedRawTransform( toolCalibratedRawTransform );
        
      // T ' = R^-1 * T * C
      //
      // where:
      // " T " is the raw transform reported by the device,
      // " C " is the tool calibration transform.
      // " R^-1 " is the inverse of the transform for the reference tool,
      // applying ReferenceTool
      
      TransformType toolCalibratedRawTransformWRTReferencetrackertool;
      if ( m_ApplyingReferenceTool )
        {
        // since this is an inverse transform, apply translation first
        TransformType::VersorType inverseRotation =
          m_ReferenceTool->GetRawTransform().GetRotation().GetReciprocal();

        translation -= m_ReferenceTool->GetRawTransform().GetTranslation();
        translation = inverseRotation.Transform(translation);
        rotation = inverseRotation*rotation;

        // also include the reference tool's ToolCalibrationTransform
        inverseRotation = m_ReferenceTool->GetCalibrationTransform().
                                           GetRotation().GetReciprocal();
        translation -= m_ReferenceTool->GetCalibrationTransform().
                                           GetTranslation();
        translation = inverseRotation.Transform(translation);
        rotation = inverseRotation*rotation;

        toolCalibratedRawTransformWRTReferencetrackertool.SetTranslationAndRotation( 
                          translation, rotation,
                          transform.GetError(),
                          timeToExpiration );

        (inputItr->second)->SetCalibratedRawTransformWithRespectToReferenceTrackerTool( 
                                             toolCalibratedRawTransformWRTReferencetrackertool );
        }

      // set transfrom with respect to the reference tool
      if ( m_ApplyingReferenceTool )
        {        
        (inputItr->second)->RequestSetTransformAndParent( 
                                      toolCalibratedRawTransformWRTReferencetrackertool, m_ReferenceTool.GetPointer() );
        }
      else
        {
        (inputItr->second)->RequestSetTransformAndParent( toolCalibratedRawTransform, this );
        }
      }
    ++inputItr;
    }

  this->InvokeEvent( TrackerNewUpdateStatusEvent() );  
}

/** This method is called when a call to RequestUpdateStatus failed */
void TrackerNew::UpdateStatusFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::UpdateStatusFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerNewUpdateStatusErrorEvent() );  
}


/** The "CloseFromTrackingStateProcessing" method closes tracker in
 *  use, when the tracker is in tracking state. */
void TrackerNew::CloseFromTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::CloseFromTrackingStateProcessing "
                 "called ...\n");

  // leaving TrackingState, going to AttemptingToCloseState
  this->ExitTrackingStateProcessing();

  ResultType result = this->InternalStopTracking();

  // detach all the tracker tools from the tracker
  this->DetachAllTrackerToolsFromTracker();
  
  if( result == SUCCESS )
    {
    result = this->InternalClose();
    }

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Detach all tracker tools from the tracker */
void TrackerNew::DetachAllTrackerToolsFromTracker()
{

  typedef TrackerToolsContainerType::iterator  InputConstIterator;

  InputConstIterator inputItr = m_TrackerTools.begin();  
  InputConstIterator inputEnd = m_TrackerTools.end();

  while( inputItr != inputEnd )
    {
    this->RemoveTrackerToolFromInternalDataContainers( inputItr->first ); 
    ++inputItr;
    }

  m_TrackerTools.clear();
}
 
/** The "CloseFromCommunicatingStateProcessing" method closes
 *  tracker in use, when the tracker is in communicating state. */
void TrackerNew::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "CloseFromCommunicatingStateProcessing called ...\n");

  // Detach all the tracker tools from the tracker
  this->DetachAllTrackerToolsFromTracker();
  
  ResultType result = this->InternalClose();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after close tracking has been successful. */ 
void TrackerNew::CloseCommunicationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "CloseCommunicationSuccessProcessing called ...\n");

  this->InvokeEvent( TrackerNewCloseEvent() );
}

/** Post-processing after close tracking has failed. */ 
void TrackerNew::CloseCommunicationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "CloseCommunicationFailureProcessing called ...\n");

  this->InvokeEvent( TrackerNewCloseErrorEvent() );
}

/** Print object information */
void TrackerNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_PulseGenerator )
    {
    os << indent << this->m_PulseGenerator << std::endl;
    }

  if( this->m_PulseObserver )
    {
    os << indent << this->m_PulseObserver << std::endl;
    }

  os << indent << "ValidityTime: " << this->m_ValidityTime << std::endl;
}

/** Request adding a tool to the tracker  */
void
TrackerNew::
RequestAttachTool( std::string trackerToolIdentifier, TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "RequestAttachTool called ...\n");
 
  m_IdentifierForTrackerToolToBeAttached = trackerToolIdentifier; 
  m_TrackerToolToBeAttached = trackerTool;
  
  igstkPushInputMacro( AttachTrackerTool );
  this->m_StateMachine.ProcessInputs();
}

/** The "AttemptToAttachTrackerToolProcessing" method attempts to
 * add a tracker tool to the tracker */
void TrackerNew::AttemptToAttachTrackerToolProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::TrackerNew::AttemptToAttachTrackerToolProcessing called ...\n");

  // Verify the tracker tool information before adding it to the
  // tracker. The conditions that need be verified depend on 
  // the tracker type. For example, for MicronTracker, the 
  // the tracker should have access to the template file of the
  // Marker that is attached to the tracker tool. 
  ResultType result = VerifyTrackerToolInformation( m_TrackerToolToBeAttached );
 
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Request remove a tool from the tracker  */
TrackerNew::ResultType 
TrackerNew::
RequestRemoveTool( std::string trackerToolIdentifier )
{
  this->m_TrackerTools.erase( trackerToolIdentifier );
  this->RemoveTrackerToolFromInternalDataContainers( trackerToolIdentifier ); 
  return SUCCESS;
}

/** This method will be overriden in the derived classes. Different types of trackers
 * contain different internal tool containers */
TrackerNew::ResultType 
TrackerNew::
RemoveTrackerToolFromInternalDataContainers( std::string trackerToolIdentifier ) 
{
  return SUCCESS;
}

TrackerNew::TrackerToolsContainerType 
TrackerNew::GetTrackerToolContainer() const
{
  return m_TrackerTools;
}

/** Verify tracker tool information*/
TrackerNew::ResultType TrackerNew
::VerifyTrackerToolInformation( TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "VerifyTrackerToolInformation called ...\n");

  //This method will be overridden in the derived classes.
  //as the tracker tool information is different for different tracker types
  return SUCCESS;
}

/** Thread function for tracking */
ITK_THREAD_RETURN_TYPE TrackerNew::TrackingThreadFunction(void* pInfoStruct)
{
  struct itk::MultiThreader::ThreadInfoStruct * pInfo = 
    (struct itk::MultiThreader::ThreadInfoStruct*)pInfoStruct;

  if( pInfo == NULL )
    {
    return ITK_THREAD_RETURN_VALUE;
    }

  if( pInfo->UserData == NULL )
    {
    return ITK_THREAD_RETURN_VALUE;
    }

  TrackerNew *pTracker = (TrackerNew*)pInfo->UserData;

  // counters for error rates
  unsigned long errorCount = 0;
  unsigned long totalCount = 0;

  int activeFlag = 1;
  while ( activeFlag )
    {
    ResultType result = pTracker->InternalThreadedUpdateStatus();
    pTracker->m_ConditionNextTransformReceived->Signal();
    
    totalCount++;
    if (result != SUCCESS)
      {
      errorCount++;
      }
      
    // check to see if we are being told to quit 
    pInfo->ActiveFlagLock->Lock();
    activeFlag = *pInfo->ActiveFlag;
    pInfo->ActiveFlagLock->Unlock();
    }
  
  igstkLogMacroStatic(pTracker, DEBUG, "TrackingThreadFunction was "
                      "terminated, " << errorCount << " errors "
                      "out of " << totalCount << "updates." << std::endl );

  return ITK_THREAD_RETURN_VALUE;
}

/** Report invalid request */
void TrackerNew::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

}
