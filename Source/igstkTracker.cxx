
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

Tracker::Tracker(void) : m_pCommunication( NULL), m_pLogger( NULL)
{
      // Connect the state machine to 'this' class.
      m_StateMachine.SetOwnerClass( this );

      // Set the state descriptors
      m_StateMachine.AddState( m_IdleState, "IdleState" );
      m_StateMachine.AddState( m_CommunicatingState, "CommunicatingState" );
      m_StateMachine.AddState( m_ToolsActiveState, "ToolsActiveState" );
      m_StateMachine.AddState( m_TrackingState, "TrackingState" );

      // Set the input descriptors
      m_StateMachine.AddInput( m_SetUpCommunication, "SetUpCommunication");
      m_StateMachine.AddInput( m_SetUpTools, "SetUpTools");
      m_StateMachine.AddInput( m_StartTracking, "StartTracking");
      m_StateMachine.AddInput( m_UpdateStatus,  "UpdateStatus");
      m_StateMachine.AddInput( m_StopTracking,  "StopTracking");
      m_StateMachine.AddInput( m_ResetTracking, "ResetTracking");
      m_StateMachine.AddInput( m_CloseTracking, "CloseTracking");

      const ActionType NoAction = 0;

      // Programming the state machine transitions
      m_StateMachine.AddTransition( m_IdleState,          m_SetUpCommunication,  m_CommunicatingState, &Tracker::SetUpCommunicationProcessing );
      m_StateMachine.AddTransition( m_CommunicatingState, m_SetUpTools,          m_ToolsActiveState,   &Tracker::SetUpToolsProcessing );
      m_StateMachine.AddTransition( m_ToolsActiveState,   m_StartTracking,       m_TrackingState,      &Tracker::StartTrackingProcessing );
      m_StateMachine.AddTransition( m_TrackingState,      m_UpdateStatus,        m_TrackingState,      &Tracker::UpdateStatusProcessing );
      m_StateMachine.AddTransition( m_TrackingState,      m_StopTracking,        m_ToolsActiveState,   &Tracker::StopTrackingProcessing );
      m_StateMachine.AddTransition( m_TrackingState,      m_ResetTracking,       m_TrackingState,      &Tracker::ResetTrackingProcessing );
      m_StateMachine.AddTransition( m_TrackingState,      m_CloseTracking,       m_IdleState,          &Tracker::CloseFromTrackingStateProcessing );
      m_StateMachine.AddTransition( m_ToolsActiveState,   m_CloseTracking,       m_IdleState,          &Tracker::CloseFromToolsActiveStateProcessing );
      m_StateMachine.AddTransition( m_CommunicatingState, m_CloseTracking,       m_IdleState,          &Tracker::CloseFromCommunicatingStateProcessing );

      m_StateMachine.SelectInitialState( m_IdleState );

      // Finish the programming and get ready to run
      m_StateMachine.SetReadyToRun();

      m_Ports.clear();
}

Tracker::~Tracker(void)
{
      m_Ports.clear();
}


void Tracker::Initialize(  const char * )
{
    igstkLogMacro( igstk::Logger::DEBUG, "SetUpCommunication called ...\n");
    this->m_StateMachine.ProcessInput( this->m_SetUpCommunication );
    igstkLogMacro( igstk::Logger::DEBUG, "SetUpTools called ...\n");
    this->m_StateMachine.ProcessInput( this->m_SetUpTools );
}


void Tracker::SetLogger( LoggerType* logger )
{
    m_pLogger = logger;
}

Tracker::LoggerType* Tracker::GetLogger(  void )
{
    return m_pLogger;
}

void Tracker::Reset( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "ResetTracking called ...\n");
    m_StateMachine.ProcessInput( m_ResetTracking );
}

void Tracker::StartTracking( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "StartTracking called ...\n");
    m_StateMachine.ProcessInput( m_StartTracking );
}

void Tracker::StopTracking( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "StopTracking called ...\n");
    m_StateMachine.ProcessInput( m_StopTracking );
}


void Tracker::UpdateStatus( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "UpdateStatus called ...\n");
    m_StateMachine.ProcessInput( m_UpdateStatus );
}


void Tracker::GetToolPosition( const int portNumber, const int toolNumber, PositionType &position ) const
{
  if ( (portNumber<=this->m_Ports.size()) &&
       (toolNumber<=this->m_Ports[portNumber].m_Tools.size()) )
  {
      position = this->m_Ports[portNumber].m_Tools[toolNumber].GetPosition();
  }
}


void Tracker::SetToolPosition( const int portNumber, const int toolNumber, const PositionType position )
{
  if ( (portNumber<=this->m_Ports.size()) &&
    (toolNumber<=this->m_Ports[portNumber].m_Tools.size()) )
  {
    this->m_Ports[portNumber].m_Tools[toolNumber].SetPosition( position );
  }
}

void Tracker::Close( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "CloseTracking called ...\n");
    m_StateMachine.ProcessInput( m_CloseTracking );
}

void Tracker::AddPort( const TrackerPortType& port)
{
    this->m_Ports.push_back( port );
}

void Tracker::ClearPorts( void )
{
    this->m_Ports.clear();
}

const Tracker::StateIdentifierType & Tracker::GetCurrentState() const
{
    return m_StateMachine.GetCurrentStateIdentifier();
}

void Tracker::SetUpCommunicationProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::SetUpCommunicationProcessing called ...\n");
}
    
void Tracker::SetUpToolsProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::SetUpToolsProcessing called ...\n");
}
    
void Tracker::StartTrackingProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::StartTrackingProcessing called ...\n");
}
    
void Tracker::UpdateStatusProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::UpdateStatusProcessing called ...\n");
}
    
void Tracker::StopTrackingProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::StopTrackingProcessing called ...\n");
}

void Tracker::ResetTrackingProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::ResetTrackingProcessing called ...\n");
}

void Tracker::DisableCommunicationProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::DisableCommunicationProcessing called ...\n");
}

void Tracker::DisableToolsProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::DisableToolsProcessing called ...\n");
}

void Tracker::CloseFromTrackingStateProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "Tracker::CloseFromTrackingStateProcessing called ...\n");
    this->StopTrackingProcessing();
    this->DisableToolsProcessing();
    this->DisableCommunicationProcessing();
}

void Tracker::CloseFromToolsActiveStateProcessing( void)
{
  igstkLogMacro( igstk::Logger::DEBUG, "Tracker::CloseFromToolsActiveStateProcessing called ...\n");
  this->DisableToolsProcessing();
  this->DisableCommunicationProcessing();
}

void Tracker::CloseFromCommunicatingStateProcessing( void )
{
  igstkLogMacro( igstk::Logger::DEBUG, "Tracker::CloseFromCommunicatingStateProcessing called ...\n");
  this->DisableCommunicationProcessing();
}

}
