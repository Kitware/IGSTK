/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkTracker.h"

namespace igstk
{

/** Constructor */
Tracker::Tracker(void) :  m_StateMachine( this ), m_Logger( NULL)
{
  // Set the state descriptors
  m_StateMachine.AddState( m_IdleState,
                           "IdleState" );

  m_StateMachine.AddState( m_AttemptingToEstablishCommunicationState,
                           "AttemptingToEstablishCommunicationState" );

  m_StateMachine.AddState( m_AttemptingToCloseCommunicationState,
                           "AttemptingToCloseCommunicationState");

  m_StateMachine.AddState( m_CommunicationEstablishedState,
                           "CommunicationEstablishedState" );

  m_StateMachine.AddState( m_AttemptingToActivateToolsState,
                           "AttemptingToActivateToolsState" );

  m_StateMachine.AddState( m_ToolsActiveState,
                           "ToolsActiveState" );

  m_StateMachine.AddState( m_AttemptingToTrackState,
                           "AttemptingToTrackState" );

  m_StateMachine.AddState( m_AttemptingToStopTrackingState,
                           "AttemptingToStopTrackingState");

  m_StateMachine.AddState( m_TrackingState,
                           "TrackingState" );

  m_StateMachine.AddState( m_AttemptingToUpdateState,
                           "AttemptingToUpdateState" );

  // Set the input descriptors
  m_StateMachine.AddInput( m_EstablishCommunicationInput,
                           "EstablishCommunicationInput");

  m_StateMachine.AddInput( m_ActivateToolsInput,
                           "ActivateToolsInput");

  m_StateMachine.AddInput( m_StartTrackingInput,
                           "StartTrackingInput");

  m_StateMachine.AddInput( m_UpdateStatusInput,
                           "UpdateStatusInput");

  m_StateMachine.AddInput( m_StopTrackingInput,
                           "StopTrackingInput");

  m_StateMachine.AddInput( m_ResetInput,
                           "ResetInput");

  m_StateMachine.AddInput( m_CloseCommunicationInput,
                           "CloseCommunicationInput");

  m_StateMachine.AddInput( m_SuccessInput,
                           "SuccessInput");

  m_StateMachine.AddInput( m_FailureInput,
                           "FailureInput");

  // Programming the state machine transitions:

  // Transitions from the IdleState
  m_StateMachine.AddTransition( m_IdleState,
                                m_EstablishCommunicationInput,
                                m_AttemptingToEstablishCommunicationState,
                                &Tracker::AttemptToOpen );

  // Transitions from the AttemptingToEstablishCommunicationState
  m_StateMachine.AddTransition( m_AttemptingToEstablishCommunicationState,
                                m_SuccessInput,
                                m_CommunicationEstablishedState,
                                &Tracker::CommunicationEstablishmentSuccessProcessing );

  m_StateMachine.AddTransition( m_AttemptingToEstablishCommunicationState,
                                m_FailureInput,
                                m_IdleState,
                                &Tracker::CommunicationEstablishmentFailureProcessing);

  // Transitions from CommunicationEstablishedState
  m_StateMachine.AddTransition( m_CommunicationEstablishedState,
                                m_ActivateToolsInput,
                                m_AttemptingToActivateToolsState,
                                &Tracker::AttemptToActivateTools );

  m_StateMachine.AddTransition( m_CommunicationEstablishedState,
                                m_CloseCommunicationInput,
                                m_AttemptingToCloseCommunicationState,
                                &Tracker::CloseFromCommunicatingStateProcessing );

  m_StateMachine.AddTransition( m_CommunicationEstablishedState,
                                m_ResetInput,
                                m_CommunicationEstablishedState,
                                &Tracker::ResetFromCommunicatingStateProcessing );

  // Transitions from AttemptingToActivateToolsState
  m_StateMachine.AddTransition( m_AttemptingToActivateToolsState,
                                m_SuccessInput,
                                m_ToolsActiveState,
                                &Tracker::ToolsActivationSuccessProcessing );

  m_StateMachine.AddTransition( m_AttemptingToActivateToolsState,
                                m_FailureInput,
                                m_CommunicationEstablishedState,
                                &Tracker::ToolsActivationFailureProcessing );

  // Transitions from ToolsActiveState
  m_StateMachine.AddTransition( m_ToolsActiveState,
                                m_StartTrackingInput,
                                m_AttemptingToTrackState,
                                &Tracker::AttemptToStartTracking );

  m_StateMachine.AddTransition( m_ToolsActiveState,
                                m_CloseCommunicationInput,
                                m_AttemptingToCloseCommunicationState,
                                &Tracker::CloseFromToolsActiveStateProcessing );

  m_StateMachine.AddTransition( m_ToolsActiveState,
                                m_ResetInput,
                                m_CommunicationEstablishedState,
                                &Tracker::ResetFromToolsActiveStateProcessing );

  // Transitions from AttemptingToTrackState
  m_StateMachine.AddTransition( m_AttemptingToTrackState,
                                m_SuccessInput,
                                m_TrackingState,
                                &Tracker::StartTrackingSuccessProcessing );

  m_StateMachine.AddTransition( m_AttemptingToTrackState,
                                m_FailureInput,
                                m_ToolsActiveState,
                                &Tracker::StartTrackingFailureProcessing );

  // Transitions from TrackingState
  m_StateMachine.AddTransition( m_TrackingState,
                                m_UpdateStatusInput,
                                m_AttemptingToUpdateState,
                                &Tracker::AttemptToUpdateStatus );

  m_StateMachine.AddTransition( m_TrackingState,
                                m_StopTrackingInput,
                                m_AttemptingToStopTrackingState,
                                &Tracker::AttemptToStopTracking );

  m_StateMachine.AddTransition( m_TrackingState,
                                m_ResetInput,
                                m_CommunicationEstablishedState,
                                &Tracker::ResetFromTrackingStateProcessing );

  m_StateMachine.AddTransition( m_TrackingState,
                                m_CloseCommunicationInput,
                                m_AttemptingToCloseCommunicationState,
                                &Tracker::CloseFromTrackingStateProcessing );

  // Transitions from AttemptingToUpdateState
  m_StateMachine.AddTransition( m_AttemptingToUpdateState,
                                m_SuccessInput,
                                m_TrackingState,
                                &Tracker::UpdateStatusSuccessProcessing );

  m_StateMachine.AddTransition( m_AttemptingToUpdateState,
                                m_FailureInput,
                                m_TrackingState,
                                &Tracker::UpdateStatusFailureProcessing );

  // Transitions from AttemptingToStopTrackingState
  m_StateMachine.AddTransition( m_AttemptingToStopTrackingState,
                                m_SuccessInput,
                                m_ToolsActiveState,
                                &Tracker::StopTrackingSuccessProcessing );

  m_StateMachine.AddTransition( m_AttemptingToStopTrackingState,
                                m_FailureInput,
                                m_TrackingState,
                                &Tracker::StopTrackingFailureProcessing );

  m_StateMachine.AddTransition( m_AttemptingToCloseCommunicationState,
                                m_SuccessInput,
                                m_IdleState,
                                &Tracker::CloseCommunicationSuccessProcessing );

  m_StateMachine.AddTransition( m_AttemptingToCloseCommunicationState,
                                m_FailureInput,
                                m_CommunicationEstablishedState,
                                &Tracker::CloseCommunicationFailureProcessing );

  // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Create a PulseGenerator object.  
  m_PulseGenerator = PulseGenerator::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & Tracker::UpdateStatus );
  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  // This is update rate for sending tracking information to the
  // spatial objects, it should be set to at least 30 Hz
  m_PulseGenerator->RequestSetFrequency( 30 );

  // By default, the reference is not used
  m_ApplyingReferenceTool = false;

  m_ConditionNextTransformReceived = itk::ConditionVariable::New();
  m_Threader = itk::MultiThreader::New();
  m_ThreadingEnabled = false;
}


/** Destructor */
Tracker::~Tracker(void)
{
  m_Ports.clear();
}


/** The "Open" method attempts to open communication with the
 *  tracking device. */
void Tracker::Open( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Open called...\n");
  this->m_StateMachine.PushInput( this->m_EstablishCommunicationInput );
  this->m_StateMachine.ProcessInputs();
}


/** The "Close" method closes communication with the device. */
void Tracker::Close( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Close called ...\n");
  m_StateMachine.PushInput( m_CloseCommunicationInput );
  m_StateMachine.ProcessInputs();
}


/** The "Initialize" method initializes a newly opened device. */
void Tracker::Initialize( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Initialize called ...\n");
  this->m_StateMachine.PushInput( this->m_ActivateToolsInput );
  this->m_StateMachine.ProcessInputs();
}


/** The "Reset" tracker method should be used to bring the tracker
  to some defined default state. */
void Tracker::Reset( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Reset called ...\n");
  m_StateMachine.PushInput( m_ResetInput );
  m_StateMachine.ProcessInputs();
}


/** The "StartTracking" method readies the tracker for tracking the
  tools connected to the tracker. */
void Tracker::StartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTracking called ...\n");
  m_StateMachine.PushInput( m_StartTrackingInput );
  m_StateMachine.ProcessInputs();
}


/** The "StopTracking" stops tracker from tracking the tools. */
void Tracker::StopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTracking called ...\n");
  m_StateMachine.PushInput( m_StopTrackingInput );
  m_StateMachine.ProcessInputs();
}


/** The "UpdateStatus" method is used for updating the status of 
  ports and tools when the tracker is in tracking state. */
void Tracker::UpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::UpdateStatus called ...\n");
  m_StateMachine.PushInput( m_UpdateStatusInput );
  m_StateMachine.ProcessInputs();
}


/** The "GetToolTransform" gets the position of tool numbered "toolNumber" on
 * port numbered "portNumber" in the variable "position". Note that this
 * variable represents the position and orientation of the tool in 3D space.
 * */
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


/** Associate a TrackerTool to an object to be tracked. This is a one-to-one
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
        objectToTrack->RequestAttachToTrackerTool( tool );
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
    This method is to be overriden by a decendent class 
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalOpen called ...\n");
  return SUCCESS;
}


/** The "InternalClose" method closes communication with a tracking device.
    This method is to be overriden by a decendent class 
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalClose called ...\n");
  return SUCCESS;
}


/** The "InternalReset" method resets tracker to a known configuration. 
    This method is to be overriden by a decendent class 
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalReset called ...\n");
  return SUCCESS;
}


/** The "InternalActivateTools" method activates tools.
    This method is to be overriden by a decendent class 
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalActivateTools called ...\n");
  return SUCCESS;
}


/** The "InternalDeactivateTools" method deactivates tools.
    This method is to be overriden by a decendent class 
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalDeactivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalDeactivateTools called ...\n");
  return SUCCESS;
}


/** The "InternalStartTracking" method starts tracking.
    This method is to be overriden by a decendent class 
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalStartTracking called ...\n");
  return SUCCESS;
}


/** The "InternalStopTracking" method stops tracking.
    This method is to be overriden by a decendent class 
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalStopTracking called ...\n");
  return SUCCESS;
}


/** The "InternalUpdateStatus" method updates tracker status.
    This method is to be overriden by a decendent class 
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalUpdateStatus called ...\n");
  return SUCCESS;
}


/** The "InternalThreadedUpdateStatus" method updates tracker status.
    This method is called in a separate thread.
    This method is to be overriden by a decendent class
    and responsible for device-specific processing */
Tracker::ResultType Tracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalThreadedUpdateStatus called ...\n");
  return SUCCESS;
}


/** The "AttemptToOpen" method attempts to open communication with a
    tracking device. */
void Tracker::AttemptToOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToOpen called ...\n");

  ResultType result = this->InternalOpen();
  
  m_StateMachine.PushInputBoolean( (bool)result,
                                   m_SuccessInput,
                                   m_FailureInput );
}


/** Post-processing after communication setup has been successful. */ 
void Tracker::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CommunicationEstablishmentSuccessProcessing called ...\n");
}


/** Post-processing after communication setup has failed. */ 
void Tracker::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CommunicationEstablishmentFailureProcessing called ...\n");
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
  igstkLogMacro( DEBUG, "igstk::Tracker::ResetFromToolsActiveStateProcessing() called ...\n");
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
void Tracker::AttemptToActivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToActivateTools called ...\n");

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
}

/** Post-processing after ports and tools setup has failed. */ 
void Tracker::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationFailureProcessing "
                 "called ...\n");
}

/** The "AttemptToStartTracking" method attempts to start tracking. */
void Tracker::AttemptToStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStartTracking called ...\n");

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
}

/** Post-processing after start tracking has failed. */ 
void Tracker::StartTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingFailureProcessing "
                 "called ...\n");
}

/** The "AttemptToStopTracking" method attempts to stop tracking. */
void Tracker::AttemptToStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStopTracking called ...\n");
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
}

/** Post-processing after start tracking has failed. */ 
void Tracker::StopTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingFailureProcessing "
                 "called ...\n");
  // going from AttemptingToStopTrackingState to TrackingState
  this->EnterTrackingStateProcessing();
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
void Tracker::AttemptToUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToUpdateStatus called ...\n");

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

        /*
          T ' = P * R^-1 * T * C

          where:
          " T " is the original tool transform reported by the device,
          " R^-1 " is the inverse of the transform for the reference tool,
          " P " is the Patient transform (it specifies the position of
                the reference with respect to patient coordinates), and
          " T ' " is the transformation that is reported to the spatial objects
          " C " is the tool calibration transform.
        */

        TransformType::VersorType rotation;
        TransformType::VectorType translation;

        // start with ToolCalibrationTransform
        rotation = toolCalibrationTransform.GetRotation();
        translation = toolCalibrationTransform.GetTranslation();

        // transform by the tracker's tool transform
        rotation *= transform.GetRotation();
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
          rotation *= inverseRotation;

          // also include the reference tool's ToolCalibrationTransform
          inverseRotation = m_ReferenceTool->GetToolCalibrationTransform().
                                             GetRotation().GetReciprocal();
          translation -= m_ReferenceTool->GetToolCalibrationTransform().
                                             GetTranslation();
          translation = inverseRotation.Transform(translation);
          rotation *= inverseRotation;
          }

        // applying PatientTransform
        rotation *= m_PatientTransform.GetRotation();
        translation = m_PatientTransform.GetRotation().Transform(translation);
        translation += m_PatientTransform.GetTranslation();

        TransformType toolTransform;
        toolTransform.SetTranslationAndRotation(translation, rotation,
                          transform.GetError(),
                          transform.GetExpirationTime());

        tool->SetTransform( toolTransform );
        }
      }
    }  
}

/** This method is called when a call to UpdateStatus failed */
void Tracker::UpdateStatusFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::UpdateStatusFailureProcessing "
                 "called ...\n");
}


/** The "CloseFromTrackingStateProcessing" method closes tracker in
    use, when the tracker is in tracking state. */
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
    in use, when the tracker is in active tools state. */
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
    tracker in use, when the tracker is in communicating state. */
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
}

/** Post-processing after close tracking has failed. */ 
void Tracker::CloseCommunicationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::"
                 "CloseCommunicationFailureProcessing called ...\n");
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

  os << indent << "Number of ports: " << this->m_Ports.size() << std::endl;
  for(unsigned int i=0; i < m_Ports.size(); ++i )
  {
    if( this->m_Ports[i] )
    {
      os << indent << *this->m_Ports[i] << std::endl;
    }
  }
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

  T ' = W * R^-1 * T * C

  where:
  " T " is the original tool transform reported by the device,
  " R^-1 " is the inverse of the transform for the reference tool,
  " W " is the Patient transform (it specifies the position of the reference
  with respect to patient coordinates), and
  " T ' " is the transformation that is reported to the spatial objects
  " C " is the tool calibration transform.
*/
void Tracker::SetPatientTransform( const PatientTransformType& transform )
{
  m_PatientTransform = transform;
}


/** The "GetPatientTransform" gets PatientTransform. */
Tracker::PatientTransformType Tracker::GetPatientTransform() const
{
  return m_PatientTransform;
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
