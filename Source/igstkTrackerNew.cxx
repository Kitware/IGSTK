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
  igstkAddStateMacro( CommunicationEstablished );
  igstkAddStateMacro( AttemptingToActivateTools ); 
  igstkAddStateMacro( ToolsActive ); 
  igstkAddStateMacro( AttemptingToTrack ); 
  igstkAddStateMacro( AttemptingToStopTracking); 
  igstkAddStateMacro( Tracking ); 
  igstkAddStateMacro( AttemptingToUpdate ); 
  
  // Set the input descriptors
  igstkAddInputMacro( EstablishCommunication);
  igstkAddInputMacro( ActivateTools); 
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
                           ActivateTools,
                           AttemptingToActivateTools,
                           AttemptToActivateTools );

  igstkAddTransitionMacro( CommunicationEstablished,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromCommunicatingState );

  igstkAddTransitionMacro( CommunicationEstablished,
                           Reset,
                           CommunicationEstablished,
                           ResetFromCommunicatingState );

  // Transitions from AttemptingToActivateTools
  igstkAddTransitionMacro( AttemptingToActivateTools,
                           Success,
                           ToolsActive,
                           ToolsActivationSuccess );

  igstkAddTransitionMacro( AttemptingToActivateTools,
                           Failure,
                           CommunicationEstablished,
                           ToolsActivationFailure );

  // Transitions from ToolsActive
  igstkAddTransitionMacro( ToolsActive,
                           StartTracking,
                           AttemptingToTrack,
                           AttemptToStartTracking );

  igstkAddTransitionMacro( ToolsActive,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseFromToolsActiveState );

  igstkAddTransitionMacro( ToolsActive,
                           Reset,
                           CommunicationEstablished,
                           ResetFromToolsActiveState );

  // Transitions from AttemptingToTrack
  igstkAddTransitionMacro( AttemptingToTrack,
                           Success,
                           Tracking,
                           StartTrackingSuccess );

  igstkAddTransitionMacro( AttemptingToTrack,
                           Failure,
                           ToolsActive,
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
                           ToolsActive,
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
  m_Ports.clear();
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


/** The "RequestInitialize" method initializes a newly opened device.
 *  FIXME: */

void TrackerNew::RequestInitialize( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::RequestInitialize called ...\n");
  igstkPushInputMacro( ActivateTools );
  this->m_StateMachine.ProcessInputs();
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

/** The "AddPort" method adds a port to the tracker. */
void TrackerNew::AddPort( TrackerPortType * port )
{
  TrackerPortPointer portPtr = port;
  this->m_Ports.push_back( portPtr );
}

/** The "ClearPorts" clears all the ports. */
void TrackerNew::ClearPorts( void )
{
  this->m_Ports.clear();
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


/** The "InternalActivateTools" method activates tools.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalActivateTools called ...\n");
  return SUCCESS;
}


/** The "InternalDeactivateTools" method deactivates tools.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
TrackerNew::ResultType TrackerNew::InternalDeactivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::InternalDeactivateTools called ...\n");
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

/** The "AttemptToActivateTools" method attempts to activate tools. */
void TrackerNew::AttemptToActivateToolsProcessing( void )
{
  igstkLogMacro( DEBUG,
           "igstk::TrackerNew::AttemptToActivateToolsProcessing called ...\n");

  ResultType result = this->InternalActivateTools();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
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
    if ( (inputItr->second)->GetUpdated() ) 
      {
      TransformType transform = (inputItr->second)->GetRawTransform();

      TransformType::VersorType rotation;
      TransformType::VectorType translation;

      // Get the rotation and translation with ToolCalibrationTransform
      rotation = transform.GetRotation();
      translation = transform.GetTranslation();

      // FIXME: IS THIS NECESSARY
      // applying ReferenceTool

      const double timeToExpiration = transform.GetExpirationTime() - 
                                      transform.GetStartTime();

      TransformType toolTransform;
      toolTransform.SetTranslationAndRotation( translation, rotation,
                        transform.GetError(),
                        timeToExpiration );

      (inputItr->second)->RequestSetTransform( toolTransform );
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

  if( result == SUCCESS )
    {
    result = this->InternalDeactivateTools();
    if ( result == SUCCESS )
      {
      result = this->InternalClose();
      }
    }

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** The "CloseFromToolsActiveStateProcessing" method closes tracker
 *  in use, when the tracker is in active tools state. */
void TrackerNew::CloseFromToolsActiveStateProcessing( void)
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "CloseFromToolsActiveStateProcessing called ...\n");

  ResultType result = this->InternalDeactivateTools();

  if ( result == SUCCESS )
    {
    result = this->InternalClose();
    }

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** The "CloseFromCommunicatingStateProcessing" method closes
 *  tracker in use, when the tracker is in communicating state. */
void TrackerNew::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "CloseFromCommunicatingStateProcessing called ...\n");

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

  os << indent << "Number of ports: " << this->m_Ports.size() << std::endl;
  for(unsigned int i=0; i < m_Ports.size(); ++i )
    {
    if( this->m_Ports[i] )
      {
      os << indent << *this->m_Ports[i] << std::endl;
      }
    }
}

/** Request adding a tool to the tracker  */
TrackerNew::ResultType 
TrackerNew::
RequestAddTool( std::string trackerToolIdentifier, TrackerToolType * trackerTool )
{
  igstkLogMacro( DEBUG, "igstk::TrackerNew::"
                 "RequestAddTool called ...\n");

  // Verify the tracker tool information before adding it to the
  // tracker. The conditions that need be verified depend on 
  // the tracker type. For example, for MicronTracker, the 
  // the tracker should have access to the template file of the
  // Marker that is attached to the tracker tool. 
  if ( VerifyTrackerToolInformation( trackerTool ) )
    {
    m_TrackerTools[ trackerToolIdentifier ] = trackerTool; 
    return SUCCESS;
    }
  else
    {
    return FAILURE;
    }
    
  // FIX: Add a code to attach the coordinate system of the tracker tool to 
  // the tracker. Something like
  // trackerTool->RequestAttachToSpatialObjectParent( this->m_CoordinateReferenceSystem );
}

/** Request remove a tool from the tracker  */
TrackerNew::ResultType 
TrackerNew::
RequestRemoveTool( std::string trackerToolIdentifier, TrackerToolType * trackerTool )
{
  // FIXME: remove tracker tool from the tracker
  return SUCCESS;
}

/** Set the tracker tool tranform using the unique identifier 
    * FIXME: this method SHOULD BE REMOVED once the coordinate system is properly
    * setup. This method is added to verify if transforms are correctly read from
    * the tracker.*/
void TrackerNew::SetToolTransform( std::string toolIdentifier, TransformType transform ) 
{     
  // check if a tracker tool with this identifier is stored in the container
  m_TrackerTools[ toolIdentifier ]->RequestSetTransform( transform );
}

/** Get the tracker tool tranform using the unique identifier 
    * FIXME: this method SHOULD BE REMOVED once the coordinate system is properly
    * setup. This method is added to debug if transforms are correctly read from
    * the tracker.*/
void TrackerNew::GetToolTransform( std::string toolIdentifier,
                                TransformType & transform )
{
  TrackerToolConstPointer tool = m_TrackerTools[ toolIdentifier ];
  transform = tool->GetTransform();
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

}
