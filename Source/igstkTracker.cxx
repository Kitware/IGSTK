
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
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

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

  m_StateMachine.AddInput( m_UpdateStatus,  "UpdateStatus");

  m_StateMachine.AddInput( m_StopTracking,  "StopTracking");
  m_StateMachine.AddInput( m_StopTrackingSuccessInput, "StopTrackingSuccessInput");
  m_StateMachine.AddInput( m_StopTrackingFailureInput, "StopTrackingFailureInput");

  m_StateMachine.AddInput( m_ResetTracking, "ResetTracking");

  m_StateMachine.AddInput( m_CloseTracking, "CloseTracking");
  m_StateMachine.AddInput( m_CloseTrackingSuccessInput, "CloseTrackingSuccessInput");
  m_StateMachine.AddInput( m_CloseTrackingFailureInput, "CloseTrackingFailureInput");

  const ActionType NoAction = 0;

  // Programming the state machine transitions
  m_StateMachine.AddTransition( m_IdleState, m_SetUpCommunicationInput,  m_AttemptingToEstablishCommunicationState, &Tracker::AttemptToSetUpCommunicationProcessing );
  m_StateMachine.AddTransition( m_AttemptingToEstablishCommunicationState, m_CommunicationEstablishmentSuccessInput, m_CommunicationEstablishedState, &Tracker::CommunicationEstablishmentSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToEstablishCommunicationState, m_CommunicationEstablishmentFailureInput, m_IdleState, &Tracker::CommunicationEstablishmentFailureProcessing);

  m_StateMachine.AddTransition( m_CommunicationEstablishedState, m_ActivateToolsInput, m_AttemptingToActivateToolsState, &Tracker::AttemptToSetUpToolsProcessing );
  m_StateMachine.AddTransition( m_AttemptingToActivateToolsState, m_ToolsActivationSuccessInput, m_ToolsActiveState, &Tracker::ToolsActivationSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToActivateToolsState, m_ToolsActivationFailureInput, m_CommunicationEstablishedState, &Tracker::ToolsActivationFailureProcessing );

  m_StateMachine.AddTransition( m_ToolsActiveState, m_StartTrackingInput, m_AttemptingToTrackState, &Tracker::AttemptToStartTrackingProcessing );
  m_StateMachine.AddTransition( m_AttemptingToTrackState, m_StartTrackingSuccessInput, m_TrackingState, &Tracker::StartTrackingSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToTrackState, m_StartTrackingFailureInput, m_ToolsActiveState, &Tracker::StartTrackingFailureProcessing );

  m_StateMachine.AddTransition( m_TrackingState, m_UpdateStatus, m_TrackingState, &Tracker::UpdateStatusProcessing );

  m_StateMachine.AddTransition( m_TrackingState, m_StopTracking, m_AttemptingToStopTrackingState, &Tracker::AttemptToStopTrackingProcessing );
  m_StateMachine.AddTransition( m_AttemptingToStopTrackingState, m_StopTrackingSuccessInput, m_ToolsActiveState, &Tracker::StopTrackingSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToStopTrackingState, m_StopTrackingFailureInput, m_TrackingState, &Tracker::StopTrackingFailureProcessing );

  m_StateMachine.AddTransition( m_TrackingState, m_ResetTracking, m_TrackingState,  &Tracker::ResetTrackingProcessing );

  m_StateMachine.AddTransition( m_TrackingState, m_CloseTracking, m_AttemptingToCloseTrackingState, &Tracker::CloseFromTrackingStateProcessing );
  m_StateMachine.AddTransition( m_ToolsActiveState, m_CloseTracking, m_AttemptingToCloseTrackingState, &Tracker::CloseFromToolsActiveStateProcessing );
  m_StateMachine.AddTransition( m_CommunicationEstablishedState, m_CloseTracking, m_AttemptingToCloseTrackingState, &Tracker::CloseFromCommunicatingStateProcessing );
  m_StateMachine.AddTransition( m_AttemptingToCloseTrackingState, m_CloseTrackingSuccessInput, m_IdleState, &Tracker::CloseTrackingSuccessProcessing );
  m_StateMachine.AddTransition( m_AttemptingToCloseTrackingState, m_CloseTrackingFailureInput, m_CommunicationEstablishedState, &Tracker::CloseTrackingFailureProcessing );

  m_StateMachine.SelectInitialState( m_IdleState );

  /** Initialize results of post-action methods */
  m_pSetUpCommunicationResultInput = &m_CommunicationEstablishmentSuccessInput;
  m_pActivateToolsResultInput = &m_ToolsActivationSuccessInput;
  m_pStartTrackingResultInput = &m_StartTrackingSuccessInput;
  m_pStopTrackingResultInput = &m_StopTrackingSuccessInput;
  m_pCloseTrackingResultInput = &m_CloseTrackingSuccessInput;

      // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();
  
  m_PulseGenerator = PulseGenerator::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & Tracker::UpdateStatusProcessing );

  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  m_PulseGenerator->RequestSetFrequency( 30 ); // 30 Hz is rather low frequency for tracking.
}

Tracker::~Tracker(void)
{
  m_Ports.clear();
}


void Tracker::Initialize(  const char * )
{
  igstkLogMacro( DEBUG, "SetUpCommunication called ...\n");
  this->m_StateMachine.PushInput( this->m_SetUpCommunicationInput );
  this->m_StateMachine.ProcessInputs();

  m_StateMachine.PushInput( *m_pSetUpCommunicationResultInput );
  m_StateMachine.ProcessInputs();

  igstkLogMacro( DEBUG, "SetUpTools called ...\n");
  this->m_StateMachine.PushInput( this->m_ActivateToolsInput );
  this->m_StateMachine.ProcessInputs();

  m_StateMachine.PushInput( *m_pActivateToolsResultInput );
  m_StateMachine.ProcessInputs();
}


void Tracker::SetLogger( LoggerType * logger )
{
  m_Logger = logger;
}

Tracker::LoggerType* Tracker::GetLogger(  void )
{
  return m_Logger;
}

void Tracker::SetCommunication( CommunicationType * communication )
{
  m_Communication = communication;
}

void Tracker::Reset( void )
{
  igstkLogMacro( DEBUG, "ResetTracking called ...\n");
  m_StateMachine.PushInput( m_ResetTracking );
  m_StateMachine.ProcessInputs();
}


void Tracker::StartTracking( void )
{
  igstkLogMacro( DEBUG, "StartTracking called ...\n");
  m_StateMachine.PushInput( m_StartTrackingInput );
  m_StateMachine.ProcessInputs();
  m_StateMachine.PushInput( *m_pStartTrackingResultInput );
  m_StateMachine.ProcessInputs();
}

void Tracker::StopTracking( void )
{
  igstkLogMacro( DEBUG, "StopTracking called ...\n");
  m_StateMachine.PushInput( m_StopTracking );
  m_StateMachine.ProcessInputs();
  m_StateMachine.PushInput( *m_pStopTrackingResultInput );
  m_StateMachine.ProcessInputs();
}


void Tracker::UpdateStatus( void )
{
  igstkLogMacro( DEBUG, "UpdateStatus called ...\n");
  m_StateMachine.PushInput( m_UpdateStatus );
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
        transitions = tool->GetTransform();
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

void Tracker::Close( void )
{
  igstkLogMacro( DEBUG, "CloseTracking called ...\n");
  m_StateMachine.PushInput( m_CloseTracking );
  m_StateMachine.ProcessInputs();
  m_StateMachine.PushInput( *m_pCloseTrackingResultInput );
  m_StateMachine.ProcessInputs();
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

void Tracker::AttemptToSetUpCommunicationProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::AttemptToSetUpCommunicationProcessing called ...\n");
}


void Tracker::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::CommunicationEstablishmentSuccessProcessing called ...\n");
}


void Tracker::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::CommunicationEstablishmentFailureProcessing called ...\n");
}


void Tracker::AttemptToSetUpToolsProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::AttemptToSetUpToolsProcessing called ...\n");
}
  

void Tracker::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::ToolsActivationSuccessProcessing called ...\n");
}
    
void Tracker::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::ToolsActivationFailureProcessing called ...\n");
}
    
void Tracker::AttemptToStartTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::AttemptToStartTrackingProcessing called ...\n");
}
   
void Tracker::StartTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::StartTrackingSuccessProcessing called ...\n");
  m_PulseGenerator->RequestStart();
}

void Tracker::StartTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::StartTrackingFailureProcessing called ...\n");
}

void Tracker::UpdateStatusProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::UpdateStatusProcessing called ...\n");
}
    
void Tracker::AttemptToStopTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::StopTrackingProcessing called ...\n");
}


void Tracker::StopTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::StopTrackingSuccessProcessing called ...\n");
  m_PulseGenerator->RequestStop();
}

void Tracker::StopTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::StopTrackingFailureProcessing called ...\n");
}

void Tracker::ResetTrackingProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::ResetTrackingProcessing called ...\n");
}

void Tracker::DisableCommunicationProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::DisableCommunicationProcessing called ...\n");
}

void Tracker::DisableToolsProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::DisableToolsProcessing called ...\n");
}

void Tracker::CloseFromTrackingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::CloseFromTrackingStateProcessing called ...\n");
  this->AttemptToStopTrackingProcessing();
  this->DisableToolsProcessing();
  this->DisableCommunicationProcessing();
}

void Tracker::CloseFromToolsActiveStateProcessing( void)
{
  igstkLogMacro( DEBUG, "Tracker::CloseFromToolsActiveStateProcessing called ...\n");
  this->DisableToolsProcessing();
  this->DisableCommunicationProcessing();
}

void Tracker::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::CloseFromCommunicatingStateProcessing called ...\n");
  this->DisableCommunicationProcessing();
}


void Tracker::CloseTrackingSuccessProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::CloseTrackingSuccessProcessing called ...\n");
}

void Tracker::CloseTrackingFailureProcessing( void )
{
  igstkLogMacro( DEBUG, "Tracker::CloseTrackingFailureProcessing called ...\n");
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


}
