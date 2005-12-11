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
// Disabling warning C4355: 'this' : used in base member initializer list
#if defined(_MSC_VER)
#pragma warning ( disable : 4355 )
#endif

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
  
  igstkAddInputMacro( ValidFrequencyInput );
  igstkAddInputMacro( InvalidLowFrequencyInput );
  igstkAddInputMacro( InvalidHighFrequencyInput );
  igstkAddInputMacro( StopInput );
  igstkAddInputMacro( StartInput );
  igstkAddInputMacro( PulseInput );
  igstkAddInputMacro( EventReturnInput );

  igstkAddStateMacro( InitialState  );
  igstkAddStateMacro( StoppedState  );
  igstkAddStateMacro( PulsingState  );
  igstkAddStateMacro( WaitingEventReturnState  );


  igstkAddTransitionMacro( InitialState, ValidFrequencyInput, StoppedState,  SetFrequency );
  igstkAddTransitionMacro( InitialState, InvalidLowFrequencyInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( InitialState, InvalidHighFrequencyInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( InitialState, StopInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( InitialState, StartInput, InitialState, ReportErrorCondition );
  igstkAddTransitionMacro( InitialState, PulseInput, InitialState, ReportErrorCondition );
  igstkAddTransitionMacro( InitialState, EventReturnInput, InitialState, ReportErrorCondition );
  
  igstkAddTransitionMacro( StoppedState, ValidFrequencyInput, StoppedState,  SetFrequency );
  igstkAddTransitionMacro( StoppedState, InvalidLowFrequencyInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( StoppedState, InvalidHighFrequencyInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( StoppedState, StopInput, StoppedState,   StopPulses );
  igstkAddTransitionMacro( StoppedState, StartInput, PulsingState, SetTimer );
  igstkAddTransitionMacro( StoppedState, PulseInput, StoppedState, ReportErrorCondition );
  igstkAddTransitionMacro( StoppedState, EventReturnInput, StoppedState, ReportErrorCondition );
 
  igstkAddTransitionMacro( PulsingState, ValidFrequencyInput, PulsingState,  SetFrequency );
  igstkAddTransitionMacro( PulsingState, InvalidLowFrequencyInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( PulsingState, InvalidHighFrequencyInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( PulsingState, StopInput, StoppedState,  StopPulses );
  igstkAddTransitionMacro( PulsingState, StartInput, PulsingState, ReportErrorCondition );
  igstkAddTransitionMacro( PulsingState, PulseInput, WaitingEventReturnState, EmitPulse );
  igstkAddTransitionMacro( PulsingState, EventReturnInput, StoppedState, ReportErrorCondition );

  igstkAddTransitionMacro( WaitingEventReturnState, ValidFrequencyInput, WaitingEventReturnState,  SetFrequency );
  igstkAddTransitionMacro( WaitingEventReturnState, InvalidLowFrequencyInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( WaitingEventReturnState, InvalidHighFrequencyInput, InitialState,  ReportErrorCondition );
  igstkAddTransitionMacro( WaitingEventReturnState, StopInput, StoppedState,  StopPulses );
  igstkAddTransitionMacro( WaitingEventReturnState, StartInput, WaitingEventReturnState, ReportErrorCondition );
  igstkAddTransitionMacro( WaitingEventReturnState, PulseInput, WaitingEventReturnState, ReportMissedPulse );
  igstkAddTransitionMacro( WaitingEventReturnState, EventReturnInput, PulsingState, NoAction );
 
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
 

/** Null operation for a State Machine transition */
void 
PulseGenerator::NoAction()
{
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

  // Process this pulse
  m_StateMachine.PushInput( m_PulseInput );
  m_StateMachine.ProcessInputs();

  // Let the state machine know that we returned from Emiting the pulse.  This
  // is important because we don't know how long it takes to accomplish the
  // operations tied to InvokeEvent(). Observers of this PulseGenerator may have
  // Execute methods that take longer than the m_Period time.
  m_StateMachine.PushInput( m_EventReturnInput );
  m_StateMachine.ProcessInputs();

  // Set the timer for the next pulse. It is rescheduled at the end of the CallbackTimer()
  // just in case the previous two ProcessInputs() calls takes a significant amount of time
  // and risk to make the PulseGenerator to miss a timer pulse.
  Fl::repeat_timeout( m_Period, 
            ::igstk::PulseGenerator::CallbackTimerGlobal, (void *)this );
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


