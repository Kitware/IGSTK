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
  m_StateMachine.AddState( m_IdleState, "IdleState" );
  m_StateMachine.AddState( m_AttemptingToEstablishCommunicationState, "AttemptingToEstablishCommunicationState" );
  m_StateMachine.AddState( m_CommunicationEstablishedState, "CommunicationEstablishedState" );
  m_StateMachine.AddState( m_AttemptingToActivateToolsState, "AttemptingToActivateToolsState" );
  m_StateMachine.AddState( m_ToolsActiveState, "ToolsActiveState" );
  m_StateMachine.AddState( m_AttemptingToTrackState, "AttemptingToTrackState" );
  m_StateMachine.AddState( m_TrackingState, "TrackingState" );
  m_StateMachine.AddState( m_AttemptingToStopTrackingState, "AttemptingToStopTrackingState");
  m_StateMachine.AddState( m_AttemptingToCloseTrackingState, "AttemptingToCloseTrackingState");

  // Set the input descriptors
  m_StateMachine.AddInput( m_SetUpCommunicationInput, "SetUpCommunicationInput");
  m_StateMachine.AddInput( m_CommunicationEstablishmentSuccessInput, "CommunicationEstablishmentSuccessInput");
  m_StateMachine.AddInput( m_CommunicationEstablishmentFailureInput, "CommunicationEstablishmentFailureInput");

  m_StateMachine.AddInput( m_ActivateToolsInput, "ActivateToolsInput");
  m_StateMachine.AddInput( m_ToolsActivationSuccessInput, "ToolsActivationSuccessInput");
  m_StateMachine.AddInput( m_ToolsActivationFailureInput, "ToolsActivationFailureInput");

  m_StateMachine.AddInput( m_StartTrackingInput, "StartTrackingInput");
  m_StateMachine.AddInput( m_StartTrackingSuccessInput, "StartTrackingSuccessInput");
  m_StateMachine.AddInput( m_StartTrackingFailureInput, "StartTrackingFailureInput");

  m_StateMachine.AddInput( m_UpdateStatusInput,  "UpdateStatus");

  m_StateMachine.AddInput( m_StopTrackingInput,  "StopTracking");
  m_StateMachine.AddInput( m_StopTrackingSuccessInput, "StopTrackingSuccessInput");
  m_StateMachine.AddInput( m_StopTrackingFailureInput, "StopTrackingFailureInput");

  m_StateMachine.AddInput( m_ResetTrackingInput, "ResetTracking");

  m_StateMachine.AddInput( m_CloseTrackingInput, "CloseTracking");
  m_StateMachine.AddInput( m_CloseTrackingSuccessInput, "CloseTrackingSuccessInput");
  m_StateMachine.AddInput( m_CloseTrackingFailureInput, "CloseTrackingFailureInput");


  // Programming the state machine transitions
  m_StateMachine.AddTransition( m_IdleState, m_SetUpCommunicationInput,  m_AttemptingToEstablishCommunicationState, &Tracker::AttemptToOpen );
  m_StateMachine.AddTransition( m_AttemptingToEstablishCommunicationState, m_CommunicationEstablishmentSuccessInput, m_CommunicationEstablishedState, &Tracker::CommunicationEstablishmentSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToEstablishCommunicationState, m_CommunicationEstablishmentFailureInput, m_IdleState, &Tracker::CommunicationEstablishmentFailureProcessing);

  m_StateMachine.AddTransition( m_CommunicationEstablishedState, m_ActivateToolsInput, m_AttemptingToActivateToolsState, &Tracker::AttemptToActivateTools );
  m_StateMachine.AddTransition( m_AttemptingToActivateToolsState, m_ToolsActivationSuccessInput, m_ToolsActiveState, &Tracker::ToolsActivationSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToActivateToolsState, m_ToolsActivationFailureInput, m_CommunicationEstablishedState, &Tracker::ToolsActivationFailureProcessing );

  m_StateMachine.AddTransition( m_ToolsActiveState, m_StartTrackingInput, m_AttemptingToTrackState, &Tracker::AttemptToStartTracking );
  m_StateMachine.AddTransition( m_AttemptingToTrackState, m_StartTrackingSuccessInput, m_TrackingState, &Tracker::StartTrackingSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToTrackState, m_StartTrackingFailureInput, m_ToolsActiveState, &Tracker::StartTrackingFailureProcessing );

  m_StateMachine.AddTransition( m_TrackingState, m_UpdateStatusInput, m_TrackingState, &Tracker::AttemptToUpdateStatus );

  m_StateMachine.AddTransition( m_TrackingState, m_StopTrackingInput, m_AttemptingToStopTrackingState, &Tracker::AttemptToStopTracking );
  m_StateMachine.AddTransition( m_AttemptingToStopTrackingState, m_StopTrackingSuccessInput, m_ToolsActiveState, &Tracker::StopTrackingSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToStopTrackingState, m_StopTrackingFailureInput, m_TrackingState, &Tracker::StopTrackingFailureProcessing );

  m_StateMachine.AddTransition( m_TrackingState, m_ResetTrackingInput, m_TrackingState,  &Tracker::AttemptToReset );

  m_StateMachine.AddTransition( m_TrackingState, m_CloseTrackingInput, m_AttemptingToCloseTrackingState, &Tracker::CloseFromTrackingStateProcessing );
  m_StateMachine.AddTransition( m_ToolsActiveState, m_CloseTrackingInput, m_AttemptingToCloseTrackingState, &Tracker::CloseFromToolsActiveStateProcessing );
  m_StateMachine.AddTransition( m_CommunicationEstablishedState, m_CloseTrackingInput, m_AttemptingToCloseTrackingState, &Tracker::CloseFromCommunicatingStateProcessing );
  m_StateMachine.AddTransition( m_AttemptingToCloseTrackingState, m_CloseTrackingSuccessInput, m_IdleState, &Tracker::CloseTrackingSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToCloseTrackingState, m_CloseTrackingFailureInput, m_CommunicationEstablishedState, &Tracker::CloseTrackingFailureProcessing );

  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Create a PulseGenerator object.  
  m_PulseGenerator = PulseGenerator::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & Tracker::AttemptToUpdateStatus );

  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  m_PulseGenerator->RequestSetFrequency( 30 ); // 30 Hz is rather low frequency for tracking.

  // For default, the reference tool's transform is not applied to other tool transforms.
  m_ApplyingReferenceTool = false;
}


/** Destructor */
Tracker::~Tracker(void)
{
  m_Ports.clear();
}


/** The "Open" method attempts to open communication with the tracking device. */
void Tracker::Open( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Open called...\n");
  this->m_StateMachine.PushInput( this->m_SetUpCommunicationInput );
  this->m_StateMachine.ProcessInputs();
}


/** The "Close" method closes communication with the device. */
void Tracker::Close( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Close called ...\n");
  m_StateMachine.PushInput( m_CloseTrackingInput );
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
  m_StateMachine.PushInput( m_ResetTrackingInput );
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
void Tracker::GetToolTransform( unsigned int portNumber, unsigned int toolNumber, TransformType &transitions ) const
{
  if ( portNumber < this->m_Ports.size()  )
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    if ( port.IsNotNull() )
      {
      if( toolNumber < port->GetNumberOfTools() )
        {
        TrackerToolConstPointer tool = port->GetTool( toolNumber );
        TransformType toolTransform = tool->GetTransform();


        transitions = toolTransform;

        /*
          T ' = W * R^-1 * T * C

          where:
          " T " is the original tool transform reported by the device,
          " R^-1 " is the inverse of the transform for the reference tool,
          " W " is the Patient transform (it specifies the position of the reference
          with respect to patient coordinates), and
          " T ' " is the transformation that is reported to the spatial objects
          " C " is the tool calibration transform.
        */

        TransformType tempT;
        TransformType::VersorType rotation;
        TransformType::VectorType translation;
        tempT = toolTransform;

        // applying ReferenceTool
        if( m_ApplyingReferenceTool )
          {
          rotation = tempT.GetRotation();
          rotation /= m_ReferenceTool->GetTransform().GetRotation();
          translation = tempT.GetTranslation();
          translation -= m_ReferenceTool->GetTransform().GetTranslation();
          tempT.SetTranslationAndRotation(translation, rotation, 
            toolTransform.GetError(), toolTransform.GetExpirationTime());
          }

        // applying PatientTransform
        rotation = m_PatientTransform.GetRotation();
        translation = m_PatientTransform.GetTranslation();
        rotation *= tempT.GetRotation();
        translation += tempT.GetTranslation();
        tempT.SetTranslationAndRotation(translation, rotation,
          toolTransform.GetError(), toolTransform.GetExpirationTime());

        // applying ToolCalibrationTransform
        rotation = tempT.GetRotation();
        translation = tempT.GetTranslation();
        rotation *= m_ToolCalibrationTransform.GetRotation();
        translation += m_ToolCalibrationTransform.GetTranslation();
        tempT.SetTranslationAndRotation(translation, rotation,
          toolTransform.GetError(), toolTransform.GetExpirationTime());

        transitions = tempT;
        }
      }
    }
}


/** The "SetToolTransform" sets the position of tool numbered "toolNumber" on
 * port numbered "portNumber" by the content of variable "position". Note
 * that this variable represents the position and orientation of the tool in
 * 3D space.  */
void Tracker::SetToolTransform( unsigned int portNumber, unsigned int toolNumber, const TransformType & transform )
{

  if ( portNumber < this->m_Ports.size()  )
    {
    TrackerPortPointer port = this->m_Ports[ portNumber ];
    if ( port.IsNotNull() )
      {
      if( toolNumber < port->GetNumberOfTools() )
        {
        TrackerToolPointer tool = port->GetTool( toolNumber );
        tool->SetTransform( transform );
        }
      }
    }

}

/** Associate a TrackerTool to an object to be tracked. This is a one-to-one
 * association and cannot be changed during the life of the application */
void Tracker::AttachObjectToTrackerTool( 
        unsigned int portNumber, unsigned int toolNumber, SpatialObject * objectToTrack )
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


/** The "AttemptToOpen" method attempts to open communication with a
    tracking device. */
void Tracker::AttemptToOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToOpen called ...\n");
  ResultType result;
  result = InternalOpen();
  
  m_StateMachine.PushInputBoolean( (bool)result, m_CommunicationEstablishmentSuccessInput,
    m_CommunicationEstablishmentFailureInput );
}


/** Post-processing after communication setup has been successful. */ 
void Tracker::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CommunicationEstablishmentSuccessProcessing called ...\n");
}


/** Post-processing after communication setup has failed. */ 
void Tracker::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CommunicationEstablishmentFailureProcessing called ...\n");
}


/** The "AttemptToReset" method attempts to bring the tracker
    to some defined default state. */
void Tracker::AttemptToReset( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToReset called ...\n");
  ResultType result;
  result = InternalReset();
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
  ResultType result;
  result = InternalActivateTools();
  
  m_StateMachine.PushInputBoolean( (bool)result, m_ToolsActivationSuccessInput, m_ToolsActivationFailureInput );
}
  

/** Post-processing after ports and tools setup has been successful. */ 
void Tracker::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationSuccessProcessing called ...\n");
}

/** Post-processing after ports and tools setup has failed. */ 
void Tracker::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationFailureProcessing called ...\n");
}

/** The "AttemptToStartTracking" method attempts to start tracking. */
void Tracker::AttemptToStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStartTracking called ...\n");
  ResultType result;
  result = InternalStartTracking();
  
  m_StateMachine.PushInputBoolean( (bool)result, m_StartTrackingSuccessInput, m_StartTrackingFailureInput );
}

/** Post-processing after start tracking has been successful. */ 
void Tracker::StartTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingSuccessProcessing called ...\n");
  m_PulseGenerator->RequestStart();
}

/** Post-processing after start tracking has failed. */ 
void Tracker::StartTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingFailureProcessing called ...\n");
}

/** The "AttemptToStopTracking" method attempts to stop tracking. */
void Tracker::AttemptToStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStopTracking called ...\n");
  ResultType result;
  result = InternalStopTracking();
  
  m_StateMachine.PushInputBoolean( (bool)result, m_StopTrackingSuccessInput, m_StopTrackingFailureInput );
}


/** Post-processing after stop tracking has been successful. */ 
void Tracker::StopTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingSuccessProcessing called ...\n");
  m_PulseGenerator->RequestStop();
}

/** Post-processing after start tracking has failed. */ 
void Tracker::StopTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingFailureProcessing called ...\n");
}

/** The "AttemptToUpdateStatus" method attempts to update status
    during tracking. */
void Tracker::AttemptToUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToUpdateStatus called ...\n");
  ResultType result;
  result = InternalUpdateStatus();
  if( result == SUCCESS )
    {
    igstkLogMacro( DEBUG, "igstk::Tracker::InternalUpdateStatus succeeded ...\n");
    }
  else if( result == FAILURE )
    {
    igstkLogMacro( DEBUG, "igstk::Tracker::InternalUpdateStatus failed ...\n");
    }
}

/** The "CloseFromTrackingStateProcessing" method closes tracker in
    use, when the tracker is in tracking state. */
void Tracker::CloseFromTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseFromTrackingStateProcessing called ...\n");

  ResultType result;
  result = InternalStopTracking();
  if( result == SUCCESS )
    {
    result = InternalDeactivateTools();
    if ( result == SUCCESS )
      {
      result = InternalClose();
      }
    }

  m_StateMachine.PushInputBoolean( (bool)result, m_CloseTrackingSuccessInput, m_CloseTrackingFailureInput );

  // stop the pulse generator
  m_PulseGenerator->RequestStop();
}

/** The "CloseFromToolsActiveStateProcessing" method closes tracker
    in use, when the tracker is in active tools state. */
void Tracker::CloseFromToolsActiveStateProcessing( void)
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseFromToolsActiveStateProcessing called ...\n");

  ResultType result;
  result = InternalDeactivateTools();
  if ( result == SUCCESS )
    {
    result = InternalClose();
    }

  m_StateMachine.PushInputBoolean( (bool)result, m_CloseTrackingSuccessInput, m_CloseTrackingFailureInput );
}

/** The "CloseFromCommunicatingStateProcessing" method closes
    tracker in use, when the tracker is in communicating state. */
void Tracker::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseFromCommunicatingStateProcessing called ...\n");
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToClose called ...\n");
  ResultType result;
  result = InternalClose();
  m_StateMachine.PushInputBoolean( (bool)result, m_CloseTrackingSuccessInput, m_CloseTrackingFailureInput );
}


/** Post-processing after close tracking has been successful. */ 
void Tracker::CloseTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseTrackingSuccessProcessing called ...\n");
}

/** Post-processing after close tracking has failed. */ 
void Tracker::CloseTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseTrackingFailureProcessing called ...\n");
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
void Tracker::SetReferenceTool( bool applyReferenceTool, unsigned int portNumber, unsigned int toolNumber )
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
bool Tracker::GetReferenceTool( unsigned int &portNumber, unsigned int &toolNumber ) const
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
void Tracker::SetPatientTransform( const PatientTransformType& _arg )
{
  m_PatientTransform = _arg;
}


/** The "GetPatientTransform" gets PatientTransform. */
Tracker::PatientTransformType Tracker::GetPatientTransform() const
{
  return m_PatientTransform;
}


/** The "SetToolCalibrationTransform" sets the tool calibration transform */
void Tracker::SetToolCalibrationTransform( const Tracker::ToolCalibrationTransformType& _arg )
{
  m_ToolCalibrationTransform = _arg;
}


/** The "GetToolCalibrationTransform" gets the tool calibration transform */
Tracker::ToolCalibrationTransformType Tracker::GetToolCalibrationTransform() const
{
  return m_ToolCalibrationTransform;
}


}
