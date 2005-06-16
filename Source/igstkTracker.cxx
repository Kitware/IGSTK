
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

Tracker::Tracker(void) : m_Logger( NULL), m_StateMachine( this )
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

  const ActionType NoAction = 0;

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
  
  m_PulseGenerator = PulseGenerator::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & Tracker::AttemptToUpdateStatus );

  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  m_PulseGenerator->RequestSetFrequency( 30 ); // 30 Hz is rather low frequency for tracking.

//  m_ApplyingReferenceTool = false;
}

Tracker::~Tracker(void)
{
  m_Ports.clear();
}


void Tracker::SetLogger( LoggerType * logger )
{
  m_Logger = logger;
}

Tracker::LoggerType* Tracker::GetLogger(  void )
{
  return m_Logger;
}

void Tracker::Open( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Open called...\n");
  this->m_StateMachine.PushInput( this->m_SetUpCommunicationInput );
  this->m_StateMachine.ProcessInputs();
}


void Tracker::Close( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Close called ...\n");
  m_StateMachine.PushInput( m_CloseTrackingInput );
  m_StateMachine.ProcessInputs();
}


void Tracker::Initialize( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Initialize called ...\n");
  this->m_StateMachine.PushInput( this->m_ActivateToolsInput );
  this->m_StateMachine.ProcessInputs();
}


/*
void Tracker::SetCommunication( CommunicationType * communication )
{
  m_Communication = communication;
}
*/

void Tracker::Reset( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::Reset called ...\n");
  m_StateMachine.PushInput( m_ResetTrackingInput );
  m_StateMachine.ProcessInputs();
}


void Tracker::StartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTracking called ...\n");
  m_StateMachine.PushInput( m_StartTrackingInput );
  m_StateMachine.ProcessInputs();
}

void Tracker::StopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTracking called ...\n");
  m_StateMachine.PushInput( m_StopTrackingInput );
  m_StateMachine.ProcessInputs();
}


void Tracker::UpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::UpdateStatus called ...\n");
  m_StateMachine.PushInput( m_UpdateStatusInput );
  m_StateMachine.ProcessInputs();
}


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
          T ' = W * R^-1 * T
          where:
          " T " is the original tool transform reported by the device,
          " R^-1 " is the inverse of the transform for the reference tool,
          " W " is the world transform (it specifies the position of the reference
          with respect to patient coordinates), and
          " T ' " is the transformation that is reported to the spatial objects
        */
/*
        TransformType tempT;
        TransformType::VersorType rotation;
        TransformType::VectorType translation;
        tempT = toolTransform;
        if( m_ApplyingReferenceTool )
          {
          rotation = tempT.GetRotation();
          rotation /= m_ReferenceTool->GetTransform().GetRotation();
          translation = tempT.GetTranslation();
          translation -= m_ReferenceTool->GetTransform().GetTranslation();
          tempT.SetTranslationAndRotation(translation, rotation, 
            tempT.GetError(), tempT.GetExpirationTime());
          }

        // we need to reflect WorldTransform here!
        if( m_WorldTransform )
        {
          rotation = m_WorldTransform->GetVersor();
          translation = m_WorldTransform->GetTranslation();
          rotation *= tempT.GetRotation();
          translation += tempT.GetTranslation();
          tempT.SetTranslationAndRotation(translation, rotation,
            tempT.GetError(), tempT.GetExpirationTime());
        }
        transitions = tempT;
*/
        }
      }
    }
}


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


void Tracker::AddPort( TrackerPortType * port )
{
  TrackerPortPointer portPtr = port;
  this->m_Ports.push_back( portPtr );
}


void Tracker::ClearPorts( void )
{
  this->m_Ports.clear();
}


Tracker::ResultType Tracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalOpen called ...\n");
  return SUCCESS;
}


Tracker::ResultType Tracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalClose called ...\n");
  return SUCCESS;
}


Tracker::ResultType Tracker::InternalReset( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalReset called ...\n");
  return SUCCESS;
}


Tracker::ResultType Tracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalActivateTools called ...\n");
  return SUCCESS;
}


Tracker::ResultType Tracker::InternalDeactivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalDeactivateTools called ...\n");
  return SUCCESS;
}


Tracker::ResultType Tracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalStartTracking called ...\n");
  return SUCCESS;
}


Tracker::ResultType Tracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalStopTracking called ...\n");
  return SUCCESS;
}


Tracker::ResultType Tracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::InternalUpdateStatus called ...\n");
  return SUCCESS;
}


void Tracker::AttemptToOpen( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToOpen called ...\n");
  ResultType result;
  result = InternalOpen();
  
  if( result == SUCCESS )
    {
    m_StateMachine.PushInput( m_CommunicationEstablishmentSuccessInput );
    }
  else if( result == FAILURE )
    {
    m_StateMachine.PushInput( m_CommunicationEstablishmentFailureInput );
    }
}


void Tracker::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CommunicationEstablishmentSuccessProcessing called ...\n");
}


void Tracker::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CommunicationEstablishmentFailureProcessing called ...\n");
}


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


void Tracker::AttemptToActivateTools( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToActivateTools called ...\n");
  ResultType result;
  result = InternalActivateTools();
  
  if( result == SUCCESS )
    {
    m_StateMachine.PushInput( m_ToolsActivationSuccessInput );
    }
  else if( result == FAILURE )
    {
    m_StateMachine.PushInput( m_ToolsActivationFailureInput );
    }
}
  

void Tracker::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationSuccessProcessing called ...\n");
}

void Tracker::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::ToolsActivationFailureProcessing called ...\n");
}

void Tracker::AttemptToStartTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStartTracking called ...\n");
  ResultType result;
  result = InternalStartTracking();
  
  if( result == SUCCESS )
    {
    m_StateMachine.PushInput( m_StartTrackingSuccessInput );
    }
  else if( result == FAILURE )
    {
    m_StateMachine.PushInput( m_StartTrackingFailureInput );
    }
}

void Tracker::StartTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingSuccessProcessing called ...\n");
  m_PulseGenerator->RequestStart();
}

void Tracker::StartTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StartTrackingFailureProcessing called ...\n");
}

void Tracker::AttemptToStopTracking( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToStopTracking called ...\n");
  ResultType result;
  result = InternalStopTracking();
  
  if( result == SUCCESS )
    {
    m_StateMachine.PushInput( m_StopTrackingSuccessInput );
    }
  else if( result == FAILURE )
    {
    m_StateMachine.PushInput( m_StopTrackingFailureInput );
    }
}


void Tracker::StopTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingSuccessProcessing called ...\n");
  m_PulseGenerator->RequestStop();
}

void Tracker::StopTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::StopTrackingFailureProcessing called ...\n");
}

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

  if (result == SUCCESS )
    {
    m_StateMachine.PushInput( m_CloseTrackingSuccessInput );
    }
  else if( result == FAILURE )
    {
    m_StateMachine.PushInput( m_CloseTrackingFailureInput );
    }
}

void Tracker::CloseFromToolsActiveStateProcessing( void)
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseFromToolsActiveStateProcessing called ...\n");

  ResultType result;
  result = InternalDeactivateTools();
  if ( result == SUCCESS )
    {
    result = InternalClose();
    }

  if (result == SUCCESS )
    {
    m_StateMachine.PushInput( m_CloseTrackingSuccessInput );
    }
  else if( result == FAILURE )
    {
    m_StateMachine.PushInput( m_CloseTrackingFailureInput );
    }
}

void Tracker::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseFromCommunicatingStateProcessing called ...\n");
  igstkLogMacro( DEBUG, "igstk::Tracker::AttemptToClose called ...\n");
  ResultType result;
  result = InternalClose();
  if( result == SUCCESS )
    {
    m_StateMachine.PushInput( m_CloseTrackingSuccessInput );
    }
  else if( result == FAILURE )
    {
    m_StateMachine.PushInput( m_CloseTrackingFailureInput );
    }
}


void Tracker::CloseTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::Tracker::CloseTrackingSuccessProcessing called ...\n");
}

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
  for(int i=0; i < m_Ports.size(); ++i )
  {
    if( this->m_Ports[i] )
    {
      os << indent << *this->m_Ports[i] << std::endl;
    }
  }
}

/*
void Tracker::SetReferenceTool( bool applyReferenceTool, unsigned int portNumber, unsigned int toolNumber )
{
  if( applyReferenceTool == false )
    m_ApplyingReferenceTool = applyReferenceTool;
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
        }
      }
    }

}


bool Tracker::GetReferenceTool( unsigned int &portNumber, unsigned int &toolNumber ) const
{
  portNumber = m_ReferenceToolPortNumber;
  toolNumber = m_ReferenceToolNumber;
  return m_ApplyingReferenceTool;
}


void Tracker::SetWorldTransform( WorldTransformType* _arg )
{
  m_WorldTransform = _arg;
}


const Tracker::WorldTransformType* Tracker::GetWorldTransform() const
{
  return m_WorldTransform;
}

void SetToolCalibrationTransform( Tracker::ToolCalibrationTransformType* _arg )
{
}
*/
}
