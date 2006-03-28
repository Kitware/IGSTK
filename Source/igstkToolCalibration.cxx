/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolCalibration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkToolCalibration.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
ToolCalibration
::ToolCalibration():m_StateMachine(this)
{
  m_Date = "";
  m_Time = "";

  igstkAddInputMacro( Initialize );
  igstkAddInputMacro( ValidTime );
  igstkAddInputMacro( InvalidTime );
  igstkAddInputMacro( ValidDate );
  igstkAddInputMacro( InvalidDate );
  igstkAddInputMacro( RequestDate ); 
  igstkAddInputMacro( RequestTime ); 

  igstkAddStateMacro( Initialize );
  igstkAddStateMacro( ValidCalibration );
  igstkAddStateMacro( InvalidCalibration );

  // Initialize State
  igstkAddTransitionMacro( Initialize, InvalidTime, InvalidCalibration,  No ); 
  igstkAddTransitionMacro( Initialize, ValidTime, ValidCalibration,  SetTime );
  igstkAddTransitionMacro( Initialize, InvalidDate, InvalidCalibration,  No ); 
  igstkAddTransitionMacro( Initialize, ValidDate, ValidCalibration,  SetDate );
   
  // Valid Calibration
  igstkAddTransitionMacro( ValidCalibration, ValidTime, ValidCalibration,  SetTime );
  igstkAddTransitionMacro( ValidCalibration, ValidDate, ValidCalibration,  SetDate );
  igstkAddTransitionMacro( ValidCalibration, InvalidTime, InvalidCalibration,  No );
  igstkAddTransitionMacro( ValidCalibration, InvalidDate, InvalidCalibration,  No );

  // Invalid Calibration
  igstkAddTransitionMacro( InvalidCalibration, ValidDate, ValidCalibration,  SetDate );
  igstkAddTransitionMacro( InvalidCalibration, ValidTime, ValidCalibration,  SetTime );
  igstkAddTransitionMacro( InvalidCalibration, InvalidDate, InvalidCalibration,  No );
  igstkAddTransitionMacro( InvalidCalibration, InvalidTime, InvalidCalibration,  No );

  igstkAddTransitionMacro( ValidCalibration, RequestDate, ValidCalibration,  ReportDate );
  igstkAddTransitionMacro( ValidCalibration, RequestTime, ValidCalibration,  ReportTime );

  igstkSetInitialStateMacro( Initialize );

  m_StateMachine.SetReadyToRun();
}


/** Destructor */
ToolCalibration
::~ToolCalibration()
{
}

/** Null operation for a State Machine transition */
void ToolCalibration
::NoProcessing()
{
}

/** Request to get the date */
void 
ToolCalibration
::RequestGetDate()
{
  m_StateMachine.PushInput( m_RequestDateInput );
  m_StateMachine.ProcessInputs();
}

void ToolCalibration
::ReportDateProcessing() 
{
  StringEvent event;
  event.Set( this->m_Date );
  this->InvokeEvent( event );
}

/** Request to get the time */
void 
ToolCalibration
::RequestGetTime()
{
  m_StateMachine.PushInput( m_RequestTimeInput );
  m_StateMachine.ProcessInputs();
}

void ToolCalibration
::ReportTimeProcessing() 
{
  StringEvent event;
  event.Set( this->m_Time );
  this->InvokeEvent( event );
}

/** Request to set the time */
void 
ToolCalibration
::RequestSetTime(const char* time)
{
  m_TimeToSet = time;
  if( !m_TimeToSet )
    {
    igstkPushInputMacro( InvalidTime );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidTime );
    m_StateMachine.ProcessInputs();
    }
}


/** Request to set the time */
void 
ToolCalibration
::SetTimeProcessing()
{
  m_Time = m_TimeToSet;
}

/** Request to set the time */
void 
ToolCalibration
::RequestSetDate(const char* date)
{
  m_DateToSet = date;
  if( !m_DateToSet )
    {
    igstkPushInputMacro( InvalidDate );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidDate );
    m_StateMachine.ProcessInputs();
    }
}


/** Request to set the time */
void 
ToolCalibration
::SetDateProcessing()
{
  m_Date = m_DateToSet;
}


/** Print Self function */
void 
ToolCalibration
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
