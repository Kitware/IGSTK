/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTracker.cxx
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

#include "igstkTracker.h"

namespace igstk
{

/** Constructor */
Tracker::Tracker(void) :  m_StateMachine( this ) 
{
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
  m_PulseObserver->SetCallbackFunction( this, & Tracker::RequestUpdateStatus );
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

  // Create the coordinate reference system of the Tracker
  m_CoordinateReferenceSystem = CoordinateReferenceSystemType::New();
}


/** Destructor */
Tracker::~Tracker(void)
{
  m_Ports.clear();
}


/** The "RequestOpen" method attempts to open communication with the
 *  tracking device. */
void Tracker::RequestOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestOpen called...\n");
  igstkPushInputMacro( EstablishCommunication );
  this->m_StateMachine.ProcessInputs();
}


/** The "RequestClose" method closes communication with the device. */
void Tracker::RequestClose( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestClose called ...\n");
  igstkPushInputMacro( CloseCommunication );
  m_StateMachine.ProcessInputs();
}


/** The "RequestInitialize" method initializes a newly opened device. */
void Tracker::RequestInitialize( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestInitialize called ...\n");
  igstkPushInputMacro( ActivateTools );
  this->m_StateMachine.ProcessInputs();
}


/** The "RequestReset" tracker method should be used to bring the tracker
 * to some defined default state. */
void Tracker::RequestReset( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestReset called ...\n");
  igstkPushInputMacro( Reset );
  m_StateMachine.ProcessInputs();
}


/** The "RequestStartTracking" method readies the tracker for tracking the
 *  tools connected to the tracker. */
void Tracker::RequestStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestStartTracking called ...\n");
  igstkPushInputMacro( StartTracking );
  m_StateMachine.ProcessInputs();
}


/** The "RequestStopTracking" stops tracker from tracking the tools. */
void Tracker::RequestStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestStopTracking called ...\n");
  igstkPushInputMacro( StopTracking );
  m_StateMachine.ProcessInputs();
}


/** The "RequestUpdateStatus" method is used for updating the status of 
 *  ports and tools when the tracker is in tracking state. */
void Tracker::RequestUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::RequestUpdateStatus called ...\n");
  igstkPushInputMacro( UpdateStatus );
  m_StateMachine.ProcessInputs();
}


/** The "GetToolTransform" gets the position of tool numbered "toolNumber" on
 * port numbered "portNumber" in the variable "position". Note that this
 * variable represents the position and orientation of the tool in 3D space. */
void Tracker::GetToolTransform( unsigned int portNumber,
                                unsigned int toolNumber,
                                TransformType &transitions ) const
{
  if ( portNumber < this->m_Ports.size()  )
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    if ( port.IsNotNull() )
      {
      if( toolNumber < port->GetNumberOfTools() )
        {
        TrackerToolConstPointer tool = port->GetTool( toolNumber );
        transitions = tool->GetTransform();
        }
      }
    }
}

/** The "SetToolTransform" sets the position of tool numbered "toolNumber" on
 * port numbered "portNumber" by the content of variable "position". Note
 * that this variable represents the position and orientation of the tool in
 * 3D space.  */
void Tracker::SetToolTransform( unsigned int portNumber,
                                unsigned int toolNumber,
                                const TransformType & transform )
{

  if ( portNumber < this->m_Ports.size()  )
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    if ( port.IsNotNull() )
      {
      if( toolNumber < port->GetNumberOfTools() )
        {
        TrackerToolPointer tool = port->GetTool( toolNumber );
        tool->SetRawTransform( transform );
        tool->SetUpdated( true );
        }
      }
    }
}



/** DEPRECATED: This method will be removed from the Toolkit.
 *  Instead create a TrackerTool, attache the SpatialObject
 *  to the TrackerTool, and then assign the TrackerTool to
 *  this Tracker.
 *
 *  Associate a TrackerTool to an object to be tracked. This is a one-to-one
 * association and cannot be changed during the life of the application */
void Tracker::AttachObjectToTrackerTool( unsigned int portNumber,
                                         unsigned int toolNumber,
                                         SpatialObject * objectToTrack )
{
  if ( portNumber < this->m_Ports.size()  )
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    if ( port.IsNotNull() )
      {
      if( toolNumber < port->GetNumberOfTools() )
        {
        TrackerToolPointer tool = port->GetTool( toolNumber );
        // FIXME:   objectToTrack->RequestAttachToTrackerTool( tool );
        tool->RequestAttachSpatialObject( objectToTrack );
        }
      }
    }
}


/** The "AddPort" method adds a port to the tracker. */
void Tracker::AddPort( TrackerPortType * port )
{
  TrackerPortPointer portPtr = port;
  this->m_Ports.push_back( portPtr );
}

/** The "ClearPorts" clears all the ports. */
void Tracker::ClearPorts( void )
{
  this->m_Ports.clear();
}

/** The "InternalOpen" method opens communication with a tracking device.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalOpen called ...\n");
  return SUCCESS;
}

/** The "InternalClose" method closes communication with a tracking device.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalClose called ...\n");
  return SUCCESS;
}


/** The "InternalReset" method resets tracker to a known configuration. 
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalReset called ...\n");
  return SUCCESS;
}


/** The "InternalActivateTools" method activates tools.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalActivateTools called ...\n");
  return SUCCESS;
}


/** The "InternalDeactivateTools" method deactivates tools.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalDeactivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalDeactivateTools called ...\n");
  return SUCCESS;
}


/** The "InternalStartTracking" method starts tracking.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalStartTracking called ...\n");
  return SUCCESS;
}


/** The "InternalStopTracking" method stops tracking.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalStopTracking called ...\n");
  return SUCCESS;
}


/** The "InternalUpdateStatus" method updates tracker status.
 *  This method is to be overridden by a descendant class 
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalUpdateStatus called ...\n");
  return SUCCESS;
}


/** The "InternalThreadedUpdateStatus" method updates tracker status.
 *  This method is called in a separate thread.
 *  This method is to be overridden by a descendant class
 *  and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::Tracker::InternalThreadedUpdateStatus called ...\n");
  return SUCCESS;
}


/** The "AttemptToOpen" method attempts to open communication with a
 *  tracking device. */
void Tracker::AttemptToOpenProcessing( void )
{
  igstkLogMacro( DEBUG, 
                 "igstk::Tracker::AttemptToOpenProcessing called ...\n");

  ResultType result = this->InternalOpen();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after communication setup has been successful. */ 
void Tracker::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::Tracker::CommunicationEstablishmentSuccessProcessing called ...\n");

  this->InvokeEvent( TrackerOpenEvent() );
}


/** Post-processing after communication setup has failed. */ 
void Tracker::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CommunicationEstablishmentFailureProcessing called ...\n");

  this->InvokeEvent( TrackerOpenErrorEvent() );
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished state */
void Tracker::ResetFromTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "ResetFromTrackingStateProcessing() called ...\n");
  // leaving TrackingState, going to CommunicationEstablishedState
  this->ExitTrackingStateProcessing();
  this->ResetFromToolsActiveStateProcessing();
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished  state */
void Tracker::ResetFromToolsActiveStateProcessing( void )
{
  igstkLogMacro( DEBUG, 
         "igstk::Tracker::ResetFromToolsActiveStateProcessing() called ...\n");
  this->ResetFromCommunicatingStateProcessing();
}

/** The Reset methods force the tracker to the
 *  CommunicationEstablished state */
void Tracker::ResetFromCommunicatingStateProcessing( void )
{
  ResultType result = this->InternalReset();

  if( result == SUCCESS )
    {
    igstkLogMacro( DEBUG, "igstk::Tracker::InternalReset succeeded ...\n");
    }
  else if( result == FAILURE )
    {
    igstkLogMacro( DEBUG, "igstk::Tracker::InternalReset failed ...\n");
    }
}

/** The "AttemptToActivateTools" method attempts to activate tools. */
void Tracker::AttemptToActivateToolsProcessing( void )
{
  igstkLogMacro( DEBUG,
           "igstk::Tracker::AttemptToActivateToolsProcessing called ...\n");

  ResultType result = this->InternalActivateTools();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}
  

/** Post-processing after ports and tools setup has been successful. */ 
void Tracker::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationSuccessProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerInitializeEvent() );
}

/** Post-processing after ports and tools setup has failed. */ 
void Tracker::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerInitializeErrorEvent() );
}

/** The "AttemptToStartTracking" method attempts to start tracking. */
void Tracker::AttemptToStartTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStartTrackingProcessing  "
                 "called ...\n");

  ResultType result = this->InternalStartTracking();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}

/** Post-processing after start tracking has been successful. */ 
void Tracker::StartTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingSuccessProcessing "
                 "called ...\n");
  // going from AttemptingToTrackState to TrackingState
  this->EnterTrackingStateProcessing();

  this->InvokeEvent( TrackerStartTrackingEvent() );
}

/** Post-processing after start tracking has failed. */ 
void Tracker::StartTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerStartTrackingErrorEvent() );
}

/** The "AttemptToStopTracking" method attempts to stop tracking. */
void Tracker::AttemptToStopTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStopTrackingProcessing "
                        "called ...\n");
  // leaving TrackingState, going to AttemptingToStopTrackingState
  this->ExitTrackingStateProcessing();

  ResultType result = this->InternalStopTracking();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after stop tracking has been successful. */ 
void Tracker::StopTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingSuccessProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerStopTrackingEvent() );  
}

/** Post-processing after start tracking has failed. */ 
void Tracker::StopTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingFailureProcessing "
                 "called ...\n");
  // going from AttemptingToStopTrackingState to TrackingState
  this->EnterTrackingStateProcessing();

  this->InvokeEvent( TrackerStopTrackingErrorEvent() );  
}

/** Needs to be called every time when entering tracking state. */ 
void Tracker::EnterTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::EnterTrackingStateProcessing "
                 "called ...\n");

  if ( this->GetThreadingEnabled() )
    {
    m_ThreadID = m_Threader->SpawnThread( TrackingThreadFunction, this );
    }

  m_PulseGenerator->RequestStart();
}

/** Needs to be called every time when exiting tracking state. */ 
void Tracker::ExitTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ExitTrackingStateProcessing "
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
void Tracker::AttemptToUpdateStatusProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToUpdateStatusProcessing "
                        "called ...\n");

  // Set all tools to "not updated"
  unsigned int numPorts = m_Ports.size();
  for (unsigned int portNumber = 0; portNumber < numPorts; portNumber++)
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    unsigned int numTools = port->GetNumberOfTools();
    for (unsigned int toolNumber = 0; toolNumber < numTools; toolNumber++)
      {
      port->GetTool( toolNumber )->SetUpdated(false);
      }
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
void Tracker::UpdateStatusSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::UpdateStatusSuccessProcessing "
                 "called ...\n");

  // calibrate transforms for all tools that were updated
  unsigned int numPorts = m_Ports.size();
  for (unsigned int portNumber = 0; portNumber < numPorts; portNumber++)
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    unsigned int numTools = port->GetNumberOfTools();
    for (unsigned int toolNumber = 0; toolNumber < numTools; toolNumber++)
      {
      TrackerToolPointer tool = port->GetTool( toolNumber );
      
      if ( tool->GetUpdated() &&
           ( !m_ApplyingReferenceTool || m_ReferenceTool->GetUpdated() ) )
        {
        TransformType transform = tool->GetRawTransform();

        ToolCalibrationTransformType toolCalibrationTransform
                                    = tool->GetToolCalibrationTransform();

        
        // T ' = P * R^-1 * T * C
        //
        // where:
        // " T " is the original tool transform reported by the device,
        // " R^-1 " is the inverse of the transform for the reference tool,
        // " P " is the Patient transform (it specifies the position of
        //       the reference with respect to patient coordinates), and
        // " T ' " is the transformation that is reported to the spatial 
        // objects
        // " C " is the tool calibration transform.
        

        TransformType::VersorType rotation;
        TransformType::VectorType translation;

        // start with ToolCalibrationTransform
        rotation = toolCalibrationTransform.GetRotation();
        translation = toolCalibrationTransform.GetTranslation();

        // transform by the tracker's tool transform
        rotation = transform.GetRotation()*rotation;
        translation = transform.GetRotation().Transform(translation);
        translation += transform.GetTranslation();

        // applying ReferenceTool
        if ( m_ApplyingReferenceTool )
          {
          // since this is an inverse transform, apply translation first
          TransformType::VersorType inverseRotation =
            m_ReferenceTool->GetRawTransform().GetRotation().GetReciprocal();

          translation -= m_ReferenceTool->GetRawTransform().GetTranslation();
          translation = inverseRotation.Transform(translation);
          rotation = inverseRotation*rotation;

          // also include the reference tool's ToolCalibrationTransform
          inverseRotation = m_ReferenceTool->GetToolCalibrationTransform().
                                             GetRotation().GetReciprocal();
          translation -= m_ReferenceTool->GetToolCalibrationTransform().
                                             GetTranslation();
          translation = inverseRotation.Transform(translation);
          rotation = inverseRotation*rotation;
          }

        // applying PatientTransform
        rotation = m_PatientTransform.GetRotation()*rotation;
        translation = m_PatientTransform.GetRotation().Transform(translation);
        translation += m_PatientTransform.GetTranslation();

        const double timeToExpiration = transform.GetExpirationTime() - 
                                        transform.GetStartTime();

        TransformType toolTransform;
        toolTransform.SetTranslationAndRotation( translation, rotation,
                          transform.GetError(),
                          timeToExpiration );

        tool->RequestSetTransform( toolTransform );
        }
      }
    }

  this->InvokeEvent( TrackerUpdateStatusEvent() );  
}

/** This method is called when a call to RequestUpdateStatus failed */
void Tracker::UpdateStatusFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::UpdateStatusFailureProcessing "
                 "called ...\n");

  this->InvokeEvent( TrackerUpdateStatusErrorEvent() );  
}


/** The "CloseFromTrackingStateProcessing" method closes tracker in
 *  use, when the tracker is in tracking state. */
void Tracker::CloseFromTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseFromTrackingStateProcessing "
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
void Tracker::CloseFromToolsActiveStateProcessing( void)
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
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
void Tracker::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CloseFromCommunicatingStateProcessing called ...\n");

  ResultType result = this->InternalClose();

  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after close tracking has been successful. */ 
void Tracker::CloseCommunicationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CloseCommunicationSuccessProcessing called ...\n");

  this->InvokeEvent( TrackerCloseEvent() );
}

/** Post-processing after close tracking has failed. */ 
void Tracker::CloseCommunicationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CloseCommunicationFailureProcessing called ...\n");

  this->InvokeEvent( TrackerCloseErrorEvent() );
}

/** Print object information */
void Tracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
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


/** Request adding a tool to the tracker */
void Tracker::RequestAddTool( TrackerToolType * trackerTool )
{
  // FIXME: thread this in the State Machine
  m_TrackerTools.push_back( trackerTool );
  trackerTool->RequestAttachToSpatialObjectParent( this->m_CoordinateReferenceSystem );
}

/** The "SetReferenceTool" sets the reference tool. */
void Tracker::SetReferenceTool( bool applyReferenceTool,
                                unsigned int portNumber,
                                unsigned int toolNumber )
{
  if( applyReferenceTool == false )
    {
    m_ApplyingReferenceTool = applyReferenceTool;
    return;
    }
  if ( portNumber < this->m_Ports.size()  )
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    if ( port.IsNotNull() )
      {
      if( toolNumber < port->GetNumberOfTools() )
        {
        m_ReferenceTool = port->GetTool( toolNumber );
        m_ReferenceToolPortNumber = portNumber;
        m_ReferenceToolNumber = toolNumber;
        m_ApplyingReferenceTool = applyReferenceTool;
        }
      }
    }
}


/** The "GetReferenceTool" gets the reference tool.
 * If the reference tool is not applied, it returns false.
 * Otherwise, it returns true. */
bool Tracker::GetReferenceTool( unsigned int &portNumber,
                                unsigned int &toolNumber ) const
{
  portNumber = m_ReferenceToolPortNumber;
  toolNumber = m_ReferenceToolNumber;

  return m_ApplyingReferenceTool;
}


/** The "SetPatientTransform" sets PatientTransform.
 *
 *  T ' = W * R^-1 * T * C
 *
 *  where:
 *  " T " is the original tool transform reported by the device,
 *  " R^-1 " is the inverse of the transform for the reference tool,
 *  " W " is the Patient transform (it specifies the position of the reference
 *  with respect to patient coordinates), and
 *  " T ' " is the transformation that is reported to the spatial objects
 *  " C " is the tool calibration transform */
void Tracker::SetPatientTransform( const PatientTransformType& transform )
{
  m_PatientTransform = transform;
}


/** The "GetPatientTransform" gets PatientTransform. */
Tracker::PatientTransformType Tracker::GetPatientTransform() const
{
  return m_PatientTransform;
}

/** Return the coordinate system associated with this tracker */
const Tracker::CoordinateReferenceSystemType *
Tracker::GetCoordinateReferenceSystem() const
{
  return m_CoordinateReferenceSystem;
}

/** The "SetToolCalibrationTransform" sets the tool calibration transform */
void Tracker::SetToolCalibrationTransform( unsigned int portNumber,
                                           unsigned int toolNumber,
                           const ToolCalibrationTransformType& transform )
{
  if ( portNumber < this->m_Ports.size()  )
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    if ( port.IsNotNull() )
      {
      if( toolNumber < port->GetNumberOfTools() )
        {
        TrackerToolPointer tool = port->GetTool( toolNumber );
        tool->SetToolCalibrationTransform( transform );
        }
      }
    }
}


/** The "GetToolCalibrationTransform" gets the tool calibration transform */
Tracker::ToolCalibrationTransformType
Tracker::GetToolCalibrationTransform(unsigned int portNumber,
                                     unsigned int toolNumber) const
{
  ToolCalibrationTransformType transform;

  if ( portNumber < this->m_Ports.size()  )
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    if ( port.IsNotNull() )
      {
      if( toolNumber < port->GetNumberOfTools() )
        {
        TrackerToolPointer tool = port->GetTool( toolNumber );
        transform = tool->GetToolCalibrationTransform();
        }
      }
    }

  return transform;
}


/** Thread function for tracking */
ITK_THREAD_RETURN_TYPE Tracker::TrackingThreadFunction(void* pInfoStruct)
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

  Tracker *pTracker = (Tracker*)pInfo->UserData;

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
