/*=========================================================================

  Program:   PivotCalibration Guided Surgery Software Toolkit
  Module:    igstkPivotCalibrationReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkPivotCalibrationReader.h"


namespace igstk
{ 
/** Constructor */
PivotCalibrationReader
::PivotCalibrationReader():m_StateMachine(this)
{
  //Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ObjectFileNameRead );
  igstkAddStateMacro( ObjectRead );
  igstkAddStateMacro( ObjectAttemptingRead );

  /** List of  Inputs */
  igstkAddInputMacro( ReadObjectRequest );
  igstkAddInputMacro( ObjectReadingError );
  igstkAddInputMacro( ObjectReadingSuccess );
  igstkAddInputMacro( ObjectFileNameValid );
  igstkAddInputMacro( ObjectFileNameIsEmpty );
  igstkAddInputMacro( ObjectFileNameIsDirectory );
  igstkAddInputMacro( ObjectFileNameDoesNotExist );
  igstkAddInputMacro( RequestCalibration );

  igstkAddTransitionMacro( Idle, ObjectFileNameValid, ObjectFileNameRead, SetFileName );
  igstkAddTransitionMacro( Idle, ObjectFileNameIsEmpty, Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ObjectFileNameIsDirectory, Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ObjectFileNameDoesNotExist, Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ReadObjectRequest, Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectFileNameRead, ReadObjectRequest, ObjectAttemptingRead, AttemptReadObject );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameValid, ObjectFileNameRead, SetFileName );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameIsEmpty, Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameIsDirectory, Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameDoesNotExist, Idle, ReportInvalidRequest );

  igstkAddTransitionMacro( ObjectRead, RequestCalibration,
                           ObjectRead, ReportCalibration );


  //Errors related to Object reading 
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectReadingError, Idle, ReportObjectReadingError );
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectReadingSuccess, ObjectRead, ReportObjectReadingSuccess );

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
PivotCalibrationReader
::~PivotCalibrationReader()  
{
}

/* This function reports invalid requests */
void PivotCalibrationReader
::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent( ObjectInvalidRequestErrorEvent() );
}


void PivotCalibrationReader
::ReportObjectReadingErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::ReportObjectReadingErrorProcessing: called...\n");
  this->InvokeEvent( ObjectReadingErrorEvent() );
}


void PivotCalibrationReader
::ReportObjectReadingSuccessProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::ReportObjectReadingSuccessProcessing: called...\n");
  this->InvokeEvent( ObjectReadingSuccessEvent() );
}

void PivotCalibrationReader
::RequestSetFileName( const FileNameType & filename )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::RequestSetFileName called...\n");
  m_FileNameToBeSet = filename;

  if( filename.empty() )
    {
    this->m_StateMachine.PushInput( this->m_ObjectFileNameIsEmptyInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  if( !itksys::SystemTools::FileExists( filename.c_str() ) )
    {
    this->m_StateMachine.PushInput( this->m_ObjectFileNameDoesNotExistInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  if( itksys::SystemTools::FileIsDirectory( filename.c_str() ))
    {
    this->m_StateMachine.PushInput( this->m_ObjectFileNameIsDirectoryInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }
  
  this->m_StateMachine.PushInput( this->m_ObjectFileNameValidInput );
  this->m_StateMachine.ProcessInputs();
}


void PivotCalibrationReader
::SetFileNameProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::SetFileNameProcessing called...\n");
  m_FileName = m_FileNameToBeSet;
}

/** Read the spatialobject file */
void PivotCalibrationReader
::AttemptReadObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::AttemptReadObjectProcessing called...\n");
  
  if(!Superclass::ParseXML())
    {
    this->m_StateMachine.PushInput( this->m_ObjectReadingErrorInput );
    return;
    }
  this->m_StateMachine.PushInput( this->m_ObjectReadingSuccessInput );
}

void PivotCalibrationReader
::RequestGetCalibration() 
{
  m_StateMachine.PushInput( m_RequestCalibrationInput );
  m_StateMachine.ProcessInputs();
}

void PivotCalibrationReader
::ReportCalibrationProcessing() 
{
  CalibrationModifiedEvent event;
  event.Set( this->m_Calibration );
  this->InvokeEvent( event );
}

/** Request to read the object file */
void PivotCalibrationReader
::RequestReadObject()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::RequestReadObject called...\n");
  this->m_StateMachine.PushInput( this->m_ReadObjectRequestInput);
  this->m_StateMachine.ProcessInputs();
}

/** Print Self function */
void
PivotCalibrationReader
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}



} // end namespace igstk

