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
  m_ToolType = "";
  m_ToolManufacturer = "";
  m_ToolPartNumber = "";
  m_ToolSerialNumber = "";
  
  igstkAddInputMacro( Initialize );
  igstkAddInputMacro( ValidTime );
  igstkAddInputMacro( InvalidTime );
  igstkAddInputMacro( ValidDate );
  igstkAddInputMacro( InvalidDate );
  igstkAddInputMacro( RequestDate );
  igstkAddInputMacro( RequestTime );
  igstkAddInputMacro( RequestToolType );
  igstkAddInputMacro( RequestToolManufacturer );
  igstkAddInputMacro( RequestToolPartNumber );
  igstkAddInputMacro( RequestToolSerialNumber );
  igstkAddInputMacro( ValidToolType );
  igstkAddInputMacro( InvalidToolType );
  igstkAddInputMacro( ValidToolManufacturer );
  igstkAddInputMacro( InvalidToolManufacturer );
  igstkAddInputMacro( ValidToolPartNumber );
  igstkAddInputMacro( InvalidToolPartNumber );
  igstkAddInputMacro( ValidToolSerialNumber );
  igstkAddInputMacro( InvalidToolSerialNumber );

  igstkAddStateMacro( Initialize );
  igstkAddStateMacro( ValidDate );  
  igstkAddStateMacro( ValidTime );
  igstkAddStateMacro( ValidToolType );
  igstkAddStateMacro( ValidToolManufacturer );
  igstkAddStateMacro( ValidToolPartNumber );
  igstkAddStateMacro( ValidToolSerialNumber );
  igstkAddStateMacro( ValidCalibration );
  igstkAddStateMacro( InvalidCalibration );

  // Initialize State
  igstkAddTransitionMacro( Initialize, InvalidDate, InvalidCalibration,  No ); 
  igstkAddTransitionMacro( Initialize, ValidDate, ValidDate,  SetDate );
  igstkAddTransitionMacro( ValidDate, InvalidTime, InvalidCalibration,  No );
  igstkAddTransitionMacro( ValidDate, ValidTime, ValidTime,  SetTime );
  igstkAddTransitionMacro( ValidTime, InvalidToolType, 
                           InvalidCalibration,  No );
  igstkAddTransitionMacro( ValidTime, ValidToolType, 
                           ValidToolType, SetToolType );
  igstkAddTransitionMacro( ValidToolType, InvalidToolManufacturer, 
                           InvalidCalibration,  No ); 
  igstkAddTransitionMacro( ValidToolType, ValidToolManufacturer,
                           ValidToolManufacturer,  SetToolManufacturer );
  igstkAddTransitionMacro( ValidToolManufacturer, InvalidToolPartNumber, 
                           InvalidCalibration,  No ); 
  igstkAddTransitionMacro( ValidToolManufacturer, ValidToolPartNumber,
                           ValidToolPartNumber,  SetToolPartNumber );
  igstkAddTransitionMacro( ValidToolPartNumber, InvalidToolSerialNumber, 
                           InvalidCalibration,  No ); 
  igstkAddTransitionMacro( ValidToolPartNumber, ValidToolSerialNumber,
                           ValidCalibration,  SetToolSerialNumber );
  // Valid Calibration
  igstkAddTransitionMacro( ValidCalibration, RequestDate, 
                           ValidCalibration,  ReportDate );
  igstkAddTransitionMacro( ValidCalibration, RequestTime, 
                           ValidCalibration,  ReportTime );
  igstkAddTransitionMacro( ValidCalibration, RequestToolType, 
                           ValidCalibration,  ReportToolType );
  igstkAddTransitionMacro( ValidCalibration, RequestToolManufacturer, 
                           ValidCalibration,  ReportToolManufacturer );
  igstkAddTransitionMacro( ValidCalibration, RequestToolPartNumber, 
                           ValidCalibration,  ReportToolPartNumber );
  igstkAddTransitionMacro( ValidCalibration, RequestToolSerialNumber, 
                           ValidCalibration,  ReportToolSerialNumber );

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

/** Set the current transform */
void ToolCalibration::SetTransform(const TransformType & transform)
{
  m_CalibrationTransform = transform;
}

/** Request to get the date */
void ToolCalibration::RequestGetDate()
{
  m_StateMachine.PushInput( m_RequestDateInput );
  m_StateMachine.ProcessInputs();
}

void ToolCalibration::ReportDateProcessing() 
{
  StringEvent event;
  event.Set( this->m_Date );
  this->InvokeEvent( event );
}

/** Request to get the time */
void ToolCalibration::RequestGetTime()
{
  m_StateMachine.PushInput( m_RequestTimeInput );
  m_StateMachine.ProcessInputs();
}

void ToolCalibration::ReportTimeProcessing() 
{
  StringEvent event;
  event.Set( this->m_Time );
  this->InvokeEvent( event );
}

/** Request to get the tool type */
void ToolCalibration::RequestGetToolType()
{
  m_StateMachine.PushInput( m_RequestToolTypeInput );
  m_StateMachine.ProcessInputs();
}

void ToolCalibration::ReportToolTypeProcessing() 
{
  StringEvent event;
  event.Set( this->m_ToolType );
  this->InvokeEvent( event );
}


/** Request to get the tool manufacturer */
void ToolCalibration::RequestGetToolManufacturer()
{
  m_StateMachine.PushInput( m_RequestToolManufacturerInput );
  m_StateMachine.ProcessInputs();
}

void ToolCalibration::ReportToolManufacturerProcessing() 
{
  StringEvent event;
  event.Set( this->m_ToolManufacturer );
  this->InvokeEvent( event );
}

/** Request to get the tool serial number  */
void ToolCalibration::RequestGetToolSerialNumber()
{
  m_StateMachine.PushInput( m_RequestToolSerialNumberInput );
  m_StateMachine.ProcessInputs();
}

void ToolCalibration::ReportToolSerialNumberProcessing() 
{
  StringEvent event;
  event.Set( this->m_ToolSerialNumber );
  this->InvokeEvent( event );
}

/** Request to get the tool part number */
void ToolCalibration::RequestGetToolPartNumber()
{
  m_StateMachine.PushInput( m_RequestToolPartNumberInput );
  m_StateMachine.ProcessInputs();
}

void ToolCalibration::ReportToolPartNumberProcessing() 
{
  StringEvent event;
  event.Set( this->m_ToolPartNumber );
  this->InvokeEvent( event );
}

/** Request to set the time */
void ToolCalibration::RequestSetTime(const char* time)
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
void ToolCalibration::SetTimeProcessing()
{
  m_Time = m_TimeToSet;
}

/** Request to set the time */
void ToolCalibration::RequestSetDate(const char* date)
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
void ToolCalibration::SetDateProcessing()
{
  m_Date = m_DateToSet;
}

/** Request to set the Tool Type */
void ToolCalibration::RequestSetToolType(const char* toolType)
{
  m_ToolTypeToSet = toolType;
  if( !m_ToolTypeToSet )
    {
    igstkPushInputMacro( InvalidToolType );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidToolType );
    m_StateMachine.ProcessInputs();
    }
}

/** Request to set the time */
void ToolCalibration::SetToolTypeProcessing()
{
  m_ToolType = m_ToolTypeToSet;
}

/** Request to set the Tool Manufacturer */
void ToolCalibration::RequestSetToolManufacturer(const char* toolManufacturer)
{
  m_ToolManufacturerToSet = toolManufacturer;
  if( !m_ToolManufacturerToSet )
    {
    igstkPushInputMacro( InvalidToolManufacturer );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidToolManufacturer );
    m_StateMachine.ProcessInputs();
    }
}

/** Request to set the tool manufacturer */
void ToolCalibration::SetToolManufacturerProcessing()
{
  m_ToolManufacturer = m_ToolManufacturerToSet;
}

/** Request to set the Tool Serial Number */
void ToolCalibration::RequestSetToolSerialNumber(const char* toolSerialNumber)
{
  m_ToolSerialNumberToSet = toolSerialNumber;
  if( !m_ToolSerialNumberToSet )
    {
    igstkPushInputMacro( InvalidToolSerialNumber );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidToolSerialNumber );
    m_StateMachine.ProcessInputs();
    }
}

/** Request to set the Tool Serial Number */
void ToolCalibration::SetToolSerialNumberProcessing()
{
  m_ToolSerialNumber = m_ToolSerialNumberToSet;
}

/** Request to set the Tool Part Number */
void ToolCalibration::RequestSetToolPartNumber(const char* toolPartNumber)
{
  m_ToolPartNumberToSet = toolPartNumber;
  if( !m_ToolPartNumberToSet )
    {
    igstkPushInputMacro( InvalidToolPartNumber );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidToolPartNumber );
    m_StateMachine.ProcessInputs();
    }
}

/** Request to set the Tool Part Number */
void ToolCalibration::SetToolPartNumberProcessing()
{
  m_ToolPartNumber = m_ToolPartNumberToSet;
}


/** Print Self function */
void 
ToolCalibration
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Date = " << m_Date.c_str() << std::endl;
  os << indent << "Time = " << m_Time.c_str() << std::endl;
  os << indent << "Tool Type = " << m_ToolType.c_str() 
     << std::endl;
  os << indent << "Tool Manufacturer = " << m_ToolManufacturer.c_str() 
     << std::endl;
  os << indent << "Tool PartNumber = " << m_ToolPartNumber.c_str() 
     << std::endl;
  os << indent << "Tool Serial Number = " << m_ToolSerialNumber.c_str() 
     << std::endl;
  os << indent << "Transform = " << m_CalibrationTransform
     << std::endl;
}

} // end namespace igstk
