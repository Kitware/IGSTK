
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
      m_StateMachine.AddState( "IdleState" );
      m_StateMachine.AddState( "CommunicatingState" );
      m_StateMachine.AddState( "ToolsActiveState" );
      m_StateMachine.AddState( "TrackingState" );

      // Set the input descriptors
      m_StateMachine.AddInput( "SetUpCommunication");
      m_StateMachine.AddInput( "SetUpTools");
      m_StateMachine.AddInput( "StartTracking");
      m_StateMachine.AddInput( "UpdateToolStatus");
      m_StateMachine.AddInput( "StopTracking");

      const ActionType NoAction = 0;

      // Programming the state machine transitions
      m_StateMachine.AddTransition( "IdleState",          "SetUpCommunication",  "CommunicatingState", NoAction );
      m_StateMachine.AddTransition( "CommunicatingState", "SetUpTools",          "ToolsActiveState",   NoAction );
      m_StateMachine.AddTransition( "ToolsActiveState",   "StartTracking",       "TrackingState",      NoAction );
      m_StateMachine.AddTransition( "TrackingState",      "UpdateToolStatus",    "TrackingState",      NoAction );
      m_StateMachine.AddTransition( "TrackingState",      "StopTracking",        "ToolsActiveState",   NoAction );

      m_StateMachine.SelectInitialState( "IdleState");

      // Finish the programming and get ready to run
      m_StateMachine.SetReadyToRun();

      m_Ports.clear();
}

Tracker::~Tracker(void)
{
}


void Tracker::Initialize(  const char * )
{
    this->SetUpCommunication();
    this->SetUpTools();
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
}

void Tracker::SetUpCommunication( void )
{
    m_StateMachine.ProcessInput("SetUpCommunication");
    igstkLogMacro( igstk::Logger::DEBUG, "SetUpCommunication called ...\n");
}
      
void Tracker::SetUpTools( void )
{
    m_StateMachine.ProcessInput("SetUpTools");
    igstkLogMacro( igstk::Logger::DEBUG, "SetUpTools called ...\n");
}

void Tracker::StartTracking( void )
{
    m_StateMachine.ProcessInput("StartTracking");
    igstkLogMacro( igstk::Logger::DEBUG, "StartTracking called ...\n");
}

void Tracker::StopTracking( void )
{
    m_StateMachine.ProcessInput("StopTracking");
    igstkLogMacro( igstk::Logger::DEBUG, "StopTracking called ...\n");
}


void Tracker::UpdateToolStatus( void )
{
    m_StateMachine.ProcessInput("UpdateToolStatus");
    igstkLogMacro( igstk::Logger::DEBUG, "UpdateToolStatus called ...\n");
}


const Tracker::StateDescriptorType& Tracker::GetCurrentState() const
{
    return m_StateMachine.GetCurrentState();
}

}
