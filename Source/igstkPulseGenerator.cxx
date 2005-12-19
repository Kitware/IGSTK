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

  igstkAddInputMacro( ValidFrequency );
  igstkAddInputMacro( InvalidLowFrequency );
  igstkAddInputMacro( InvalidHighFrequency );
  igstkAddInputMacro( Stop );
  igstkAddInputMacro( Start );
  igstkAddInputMacro( Pulse );
  igstkAddInputMacro( EventReturn );

  igstkAddStateMacro( Initial  );
  igstkAddStateMacro( Stopped  );
  igstkAddStateMacro( Pulsing  );
  igstkAddStateMacro( WaitingEventReturn  );


  igstkAddTransitionMacro( Initial, ValidFrequency, Stopped,  SetFrequency );
  igstkAddTransitionMacro( Initial, InvalidLowFrequency, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( Initial, InvalidHighFrequency, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( Initial, Stop, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( Initial, Start, Initial, ReportErrorCondition );
  igstkAddTransitionMacro( Initial, Pulse, Initial, ReportErrorCondition );
  igstkAddTransitionMacro( Initial, EventReturn, Initial, ReportErrorCondition );
  
  igstkAddTransitionMacro( Stopped, ValidFrequency, Stopped,  SetFrequency );
  igstkAddTransitionMacro( Stopped, InvalidLowFrequency, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( Stopped, InvalidHighFrequency, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( Stopped, Stop, Stopped,   StopPulses );
  igstkAddTransitionMacro( Stopped, Start, Pulsing, SetTimer );
  igstkAddTransitionMacro( Stopped, Pulse, Stopped, ReportErrorCondition );
  igstkAddTransitionMacro( Stopped, EventReturn, Stopped, ReportErrorCondition );
 
  igstkAddTransitionMacro( Pulsing, ValidFrequency, Pulsing,  SetFrequency );
  igstkAddTransitionMacro( Pulsing, InvalidLowFrequency, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( Pulsing, InvalidHighFrequency, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( Pulsing, Stop, Stopped,  StopPulses );
  igstkAddTransitionMacro( Pulsing, Start, Pulsing, ReportErrorCondition );
  igstkAddTransitionMacro( Pulsing, Pulse, WaitingEventReturn, EmitPulse );
  igstkAddTransitionMacro( Pulsing, EventReturn, Stopped, ReportErrorCondition );

  igstkAddTransitionMacro( WaitingEventReturn, ValidFrequency, WaitingEventReturn,  SetFrequency );
  igstkAddTransitionMacro( WaitingEventReturn, InvalidLowFrequency, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( WaitingEventReturn, InvalidHighFrequency, Initial,  ReportErrorCondition );
  igstkAddTransitionMacro( WaitingEventReturn, Stop, Stopped,  StopPulses );
  igstkAddTransitionMacro( WaitingEventReturn, Start, WaitingEventReturn, ReportErrorCondition );
  igstkAddTransitionMacro( WaitingEventReturn, Pulse, WaitingEventReturn, ReportMissedPulse );
  igstkAddTransitionMacro( WaitingEventReturn, EventReturn, Pulsing, No );
 
  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();

}

PulseGenerator::~PulseGenerator()
{
}


void
PulseGenerator::RequestSetFrequency( double frequency )
{
  igstkLogMacro( DEBUG, "RequestSetFrequency() called ...\n");

  m_FrequencyToBeSet = frequency;
  if( frequency <= 0.0 )
    {
    igstkPushInputMacro( InvalidLowFrequency );
    m_StateMachine.ProcessInputs();
    return;
    }
 
  if( frequency >= m_MaximumFrequency )
    {
    igstkPushInputMacro( InvalidHighFrequency );
    m_StateMachine.ProcessInputs();
    return;
    }

  igstkPushInputMacro( ValidFrequency );
  m_StateMachine.ProcessInputs();

}


void
PulseGenerator::RequestStart()
{
  igstkLogMacro( DEBUG, "RequestStart() called ...\n");
  igstkPushInputMacro( Start );
  m_StateMachine.ProcessInputs();
}
 

void
PulseGenerator::RequestStop()
{
  igstkLogMacro( DEBUG, "RequestStop() called ...\n");
  igstkPushInputMacro( Stop );
  m_StateMachine.ProcessInputs();
}
 

/** Null operation for a State Machine transition */
void 
PulseGenerator::NoProcessing()
{
}


void
PulseGenerator::SetFrequencyProcessing()
{
  igstkLogMacro( DEBUG, "SetFrequencyProcessing() called ...\n");
  m_Frequency = m_FrequencyToBeSet;
  m_Period = 1.0 / m_Frequency;
}


void
PulseGenerator::SetTimerProcessing()
{
  igstkLogMacro( DEBUG, "SetTimerProcessing() called ...\n");
  Fl::add_timeout( m_Period, 
            ::igstk::PulseGenerator::CallbackTimerGlobal, (void *)this );
}


void
PulseGenerator::StopPulsesProcessing()
{
  igstkLogMacro( DEBUG, "StopPulsesProcessing() called ...\n");
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
  igstkPushInputMacro( Pulse );
  m_StateMachine.ProcessInputs();

  // Let the state machine know that we returned from Emiting the pulse.  This
  // is important because we don't know how long it takes to accomplish the
  // operations tied to InvokeEvent(). Observers of this PulseGenerator may have
  // Execute methods that take longer than the m_Period time.
  igstkPushInputMacro( EventReturn );
  m_StateMachine.ProcessInputs();

  // Set the timer for the next pulse. It is rescheduled at the end of the CallbackTimer()
  // just in case the previous two ProcessInputs() calls takes a significant amount of time
  // and risk to make the PulseGenerator to miss a timer pulse.
  Fl::repeat_timeout( m_Period, 
            ::igstk::PulseGenerator::CallbackTimerGlobal, (void *)this );
}


void
PulseGenerator::EmitPulseProcessing()
{
  igstkLogMacro( DEBUG, "EmitPulseProcessing() called ...\n");
  // Notify potential observers that the generator emited a pulse.
  this->InvokeEvent( PulseEvent() );
}


void
PulseGenerator::ReportErrorConditionProcessing()
{
  igstkLogMacro( WARNING, "ReportErrorConditionProcessing() called ...\n");
}


void
PulseGenerator::ReportMissedPulseProcessing()
{
  igstkLogMacro( WARNING, "ReportMissedPulseProcessing() called ...Pulse Missed !!!. It means that the frequency of the pulse generator is to high for the time needed by Observers to complete their execute method. Please reduce the frequency, of use faster Execute methjods.");
}


/** Print Self function */
void PulseGenerator::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Frequency: " << m_Frequency << std::endl;
  os << indent << "Period: " << m_Period << std::endl;
}

}


