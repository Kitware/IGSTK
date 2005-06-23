/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPulseGenerator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPulseGenerator.h"

#include <iostream>
#include <FL/Fl.H>
#include "igstkEvents.h"

namespace igstk
{

double PulseGenerator::m_MaximumFrequency = 10000; // 10 KHz

PulseGenerator::PulseGenerator():m_StateMachine(this)
{

  m_Logger = NULL;
  
  m_StateMachine.AddInput( m_ValidFrequencyInput,  "FrequencyInput" );
  m_StateMachine.AddInput( m_InvalidLowFrequencyInput,  "InvalidLowFrequencyInput" );
  m_StateMachine.AddInput( m_InvalidHighFrequencyInput,  "InvalidHighFrequencyInput" );
  m_StateMachine.AddInput( m_StopInput,  "StopInput" );
  m_StateMachine.AddInput( m_StartInput,  "StartInput" );
  m_StateMachine.AddInput( m_PulseInput,  "PulseInput" );
  m_StateMachine.AddInput( m_EventReturnInput,  "EventReturnInput" );

  m_StateMachine.AddState( m_InitialState,  "InitialState"  );
  m_StateMachine.AddState( m_StoppedState,  "StoppedState"  );
  m_StateMachine.AddState( m_PulsingState,  "PulsingState"  );
  m_StateMachine.AddState( m_WaitingEventReturnState,  "WaitingEventReturnState"  );

  const ActionType NoAction = 0;

  m_StateMachine.AddTransition( m_InitialState, m_ValidFrequencyInput, m_StoppedState,  & PulseGenerator::SetFrequency );
  m_StateMachine.AddTransition( m_InitialState, m_InvalidLowFrequencyInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_InitialState, m_InvalidHighFrequencyInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_InitialState, m_StopInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_InitialState, m_StartInput, m_InitialState, & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_InitialState, m_PulseInput, m_InitialState, & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_InitialState, m_EventReturnInput, m_InitialState, & PulseGenerator::ReportErrorCondition );
  
  m_StateMachine.AddTransition( m_StoppedState, m_ValidFrequencyInput, m_StoppedState,  & PulseGenerator::SetFrequency );
  m_StateMachine.AddTransition( m_StoppedState, m_InvalidLowFrequencyInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_StoppedState, m_InvalidHighFrequencyInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_StoppedState, m_StopInput, m_StoppedState,   & PulseGenerator::StopPulses );
  m_StateMachine.AddTransition( m_StoppedState, m_StartInput, m_PulsingState, & PulseGenerator::SetTimer );
  m_StateMachine.AddTransition( m_StoppedState, m_PulseInput, m_StoppedState, & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_StoppedState, m_EventReturnInput, m_StoppedState, & PulseGenerator::ReportErrorCondition );
 
  m_StateMachine.AddTransition( m_PulsingState, m_ValidFrequencyInput, m_PulsingState,  & PulseGenerator::SetFrequency );
  m_StateMachine.AddTransition( m_PulsingState, m_InvalidLowFrequencyInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_PulsingState, m_InvalidHighFrequencyInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_PulsingState, m_StopInput, m_StoppedState,  & PulseGenerator::StopPulses );
  m_StateMachine.AddTransition( m_PulsingState, m_StartInput, m_PulsingState, & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_PulsingState, m_PulseInput, m_WaitingEventReturnState, & PulseGenerator::EmitPulse );
  m_StateMachine.AddTransition( m_PulsingState, m_EventReturnInput, m_StoppedState, & PulseGenerator::ReportErrorCondition );

  m_StateMachine.AddTransition( m_WaitingEventReturnState, m_ValidFrequencyInput, m_WaitingEventReturnState,  & PulseGenerator::SetFrequency );
  m_StateMachine.AddTransition( m_WaitingEventReturnState, m_InvalidLowFrequencyInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_WaitingEventReturnState, m_InvalidHighFrequencyInput, m_InitialState,  & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_WaitingEventReturnState, m_StopInput, m_StoppedState,  & PulseGenerator::StopPulses );
  m_StateMachine.AddTransition( m_WaitingEventReturnState, m_StartInput, m_WaitingEventReturnState, & PulseGenerator::ReportErrorCondition );
  m_StateMachine.AddTransition( m_WaitingEventReturnState, m_PulseInput, m_WaitingEventReturnState, & PulseGenerator::ReportMissedPulse );
  m_StateMachine.AddTransition( m_WaitingEventReturnState, m_EventReturnInput, m_PulsingState, NoAction );
 
  m_StateMachine.SelectInitialState( m_InitialState );

  m_StateMachine.SetReadyToRun();

}

PulseGenerator::~PulseGenerator()
{
}


void
PulseGenerator::RequestSetFrequency( double frequency )
{
  igstkLogMacro( DEBUG, "SetFrequency() called ...\n");

  m_FrequencyToBeSet = frequency;
  if( frequency <= 0.0 )
    {
    m_StateMachine.PushInput( m_InvalidLowFrequencyInput );
    m_StateMachine.ProcessInputs();
    return;
    }
 
  if( frequency >= m_MaximumFrequency )
    {
    m_StateMachine.PushInput( m_InvalidHighFrequencyInput );
    m_StateMachine.ProcessInputs();
    return;
    }

  m_StateMachine.PushInput( m_ValidFrequencyInput );
  m_StateMachine.ProcessInputs();

}


void
PulseGenerator::RequestStart()
{
  igstkLogMacro( DEBUG, "RequestStart() called ...\n");
  m_StateMachine.PushInput( m_StartInput );
  m_StateMachine.ProcessInputs();
}
 

void
PulseGenerator::RequestStop()
{
  igstkLogMacro( DEBUG, "RequestStop() called ...\n");
  m_StateMachine.PushInput( m_StopInput );
  m_StateMachine.ProcessInputs();
}
 

void
PulseGenerator::SetFrequency()
{
  igstkLogMacro( DEBUG, "SetFrequency() called ...\n");
  m_Frequency = m_FrequencyToBeSet;
  m_Period = 1.0 / m_Frequency;
}


void
PulseGenerator::SetTimer()
{
  igstkLogMacro( DEBUG, "SetTimer() called ...\n");
  Fl::add_timeout( m_Period, 
            ::igstk::PulseGenerator::CallbackTimerGlobal, (void *)this );
}


void
PulseGenerator::StopPulses()
{
  igstkLogMacro( DEBUG, "StopPulses() called ...\n");
  Fl::remove_timeout( 
            ::igstk::PulseGenerator::CallbackTimerGlobal, (void *)this );
}


void
PulseGenerator::CallbackTimerGlobal( void *caller )
{
  // dynamic_cast<> cannot be used on void *
  // so we have to use the old style cast.
  Self * pulseGenerator = ( Self * )( caller );
  if ( pulseGenerator )
    {
    pulseGenerator->CallbackTimer();
    }
}


void
PulseGenerator::CallbackTimer()
{
  
  igstkLogMacro( DEBUG, "CallbackTimer() called ...\n");

  // Set the timer for the next pulse
  Fl::repeat_timeout( m_Period, 
            ::igstk::PulseGenerator::CallbackTimerGlobal, (void *)this );
  // Process this pulse
  m_StateMachine.PushInput( m_PulseInput );
  m_StateMachine.ProcessInputs();

  // Let the state machine know that we returned from Emiting the pulse.  This
  // is important because we don't know how long it takes to accomplish the
  // operations tied to InvokeEvent(). Observers of this PulseGenerator may have
  // Execute methods that take longer than the m_Period time.
  m_StateMachine.PushInput( m_EventReturnInput );
  m_StateMachine.ProcessInputs();
}


void
PulseGenerator::EmitPulse()
{
  igstkLogMacro( DEBUG, "EmitPulse() called ...\n");
  // Notify potential observers that the generator emited a pulse.
  this->InvokeEvent( PulseEvent() );
}


void
PulseGenerator::ReportErrorCondition()
{
  igstkLogMacro( WARNING, "ReportErrorCondition() called ...\n");
}


void
PulseGenerator::ReportMissedPulse()
{
  igstkLogMacro( WARNING, "ReportMissedPulse() called ...Pulse Missed !!!. It means that the frequency of the pulse generator is to high for the time needed by Observers to complete their execute method. Please reduce the frequency, of use faster Execute methjods.");
}


/** Print Self function */
void PulseGenerator::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Frequency: " << m_Frequency << std::endl;
  os << indent << "Period: " << m_Period << std::endl;
}

}


