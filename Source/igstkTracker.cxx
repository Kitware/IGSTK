
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

}

Tracker::~Tracker(void)
{
  m_Ports.clear();
}


void Tracker::Initialize(  const char * )
{
  igstkLogMacro( Logger::DEBUG, "SetUpCommunication called ...\n");
  this->m_StateMachine.ProcessInput( this->m_SetUpCommunicationInput );

  m_StateMachine.ProcessInput( *m_pSetUpCommunicationResultInput );

  igstkLogMacro( Logger::DEBUG, "SetUpTools called ...\n");
  this->m_StateMachine.ProcessInput( this->m_ActivateToolsInput );

  m_StateMachine.ProcessInput( *m_pActivateToolsResultInput );
}


void Tracker::SetLogger( LoggerType * logger )
{
  m_Logger = logger;
}


Tracker::LoggerType* Tracker::GetLogger(  void )
{
  return m_Logger;
}


void Tracker::Reset( void )
{
  igstkLogMacro( Logger::DEBUG, "ResetTracking called ...\n");
  m_StateMachine.ProcessInput( m_ResetTracking );
}


void Tracker::StartTracking( void )
{
  igstkLogMacro( Logger::DEBUG, "StartTracking called ...\n");
  m_StateMachine.ProcessInput( m_StartTrackingInput );
  m_StateMachine.ProcessInput( *m_pStartTrackingResultInput );
}

void Tracker::StopTracking( void )
{
  igstkLogMacro( Logger::DEBUG, "StopTracking called ...\n");
  m_StateMachine.ProcessInput( m_StopTracking );
  m_StateMachine.ProcessInput( *m_pStopTrackingResultInput );
}


void Tracker::UpdateStatus( void )
{
  igstkLogMacro( Logger::DEBUG, "UpdateStatus called ...\n");
  m_StateMachine.ProcessInput( m_UpdateStatus );
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
  igstkLogMacro( Logger::DEBUG, "CloseTracking called ...\n");
  m_StateMachine.ProcessInput( m_CloseTracking );
  m_StateMachine.ProcessInput( *m_pCloseTrackingResultInput );
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
  igstkLogMacro( Logger::DEBUG, "Tracker::AttemptToSetUpCommunicationProcessing called ...\n");
}


void Tracker::CommunicationEstablishmentSuccessProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::CommunicationEstablishmentSuccessProcessing called ...\n");
}


void Tracker::CommunicationEstablishmentFailureProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::CommunicationEstablishmentFailureProcessing called ...\n");
}


void Tracker::AttemptToSetUpToolsProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::AttemptToSetUpToolsProcessing called ...\n");
}
  

void Tracker::ToolsActivationSuccessProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::ToolsActivationSuccessProcessing called ...\n");
}
    
void Tracker::ToolsActivationFailureProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::ToolsActivationFailureProcessing called ...\n");
}
    
void Tracker::AttemptToStartTrackingProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::AttemptToStartTrackingProcessing called ...\n");
}
   
void Tracker::StartTrackingSuccessProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::StartTrackingSuccessProcessing called ...\n");
}

void Tracker::StartTrackingFailureProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::StartTrackingFailureProcessing called ...\n");
}

void Tracker::UpdateStatusProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::UpdateStatusProcessing called ...\n");
}
    
void Tracker::AttemptToStopTrackingProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::StopTrackingProcessing called ...\n");
}


void Tracker::StopTrackingSuccessProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::StopTrackingSuccessProcessing called ...\n");
}

void Tracker::StopTrackingFailureProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::StopTrackingFailureProcessing called ...\n");
}

void Tracker::ResetTrackingProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::ResetTrackingProcessing called ...\n");
}

void Tracker::DisableCommunicationProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::DisableCommunicationProcessing called ...\n");
}

void Tracker::DisableToolsProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::DisableToolsProcessing called ...\n");
}

void Tracker::CloseFromTrackingStateProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::CloseFromTrackingStateProcessing called ...\n");
  this->AttemptToStopTrackingProcessing();
  this->DisableToolsProcessing();
  this->DisableCommunicationProcessing();
}

void Tracker::CloseFromToolsActiveStateProcessing( void)
{
  igstkLogMacro( Logger::DEBUG, "Tracker::CloseFromToolsActiveStateProcessing called ...\n");
  this->DisableToolsProcessing();
  this->DisableCommunicationProcessing();
}

void Tracker::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::CloseFromCommunicatingStateProcessing called ...\n");
  this->DisableCommunicationProcessing();
}


void Tracker::CloseTrackingSuccessProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::CloseTrackingSuccessProcessing called ...\n");
}

void Tracker::CloseTrackingFailureProcessing( void )
{
  igstkLogMacro( Logger::DEBUG, "Tracker::CloseTrackingFailureProcessing called ...\n");
}



}
