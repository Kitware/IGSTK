/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#ifndef __igstkSpatialObjectReader_txx
#define __igstkSpatialObjectReader_txx

#include "igstkSpatialObjectReader.h"

namespace igstk
{ 


/** Constructor */
template <unsigned int TDimension, typename TPixelType>
SpatialObjectReader< TDimension, TPixelType >
::SpatialObjectReader() : m_StateMachine(this)
{
  // Create the ITK SpatialObject file reader
  m_SpatialObjectReader = SpatialObjectReaderType::New();

  //Set the state descriptors
  igstkAddStateMacro( IdleState );
  igstkAddStateMacro( ObjectFileNameReadState );
  igstkAddStateMacro( ObjectReadState );
  igstkAddStateMacro( ObjectAttemptingReadState );

  /** List of State Inputs */
  igstkAddInputMacro( ReadObjectRequestInput );
  igstkAddInputMacro( ObjectReadingErrorInput );
  igstkAddInputMacro( ObjectReadingSuccessInput );
  igstkAddInputMacro( ObjectFileNameValidInput );
  igstkAddInputMacro( ObjectFileNameIsEmptyInput );
  igstkAddInputMacro( ObjectFileNameIsDirectoryInput );
  igstkAddInputMacro( ObjectFileNameDoesNotExistInput );

  igstkAddTransitionMacro( IdleState, ObjectFileNameValidInput, ObjectFileNameReadState, SetFileName );
  igstkAddTransitionMacro( IdleState, ObjectFileNameIsEmptyInput, IdleState, ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, ObjectFileNameIsDirectoryInput, IdleState, ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, ObjectFileNameDoesNotExistInput, IdleState, ReportInvalidRequest );
  igstkAddTransitionMacro( IdleState, ReadObjectRequestInput, IdleState, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectFileNameReadState, ReadObjectRequestInput, ObjectAttemptingReadState, AttemptReadObject );
  igstkAddTransitionMacro( ObjectReadState, ObjectFileNameValidInput, ObjectFileNameReadState, SetFileName );
  igstkAddTransitionMacro( ObjectReadState, ObjectFileNameIsEmptyInput, IdleState, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectReadState, ObjectFileNameIsDirectoryInput, IdleState, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectReadState, ObjectFileNameDoesNotExistInput, IdleState, ReportInvalidRequest );


  //Errors related to Object reading 
  igstkAddTransitionMacro( ObjectAttemptingReadState, ObjectReadingErrorInput, IdleState, ReportObjectReadingError );
  igstkAddTransitionMacro( ObjectAttemptingReadState, ObjectReadingSuccessInput, ObjectReadState, ReportObjectReadingSuccess );

  // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

} 

/** Destructor */
template <unsigned int TDimension, typename TPixelType>
SpatialObjectReader<TDimension,TPixelType>::~SpatialObjectReader()  
{

}

/* This function reports invalid requests */
template <unsigned int TDimension, typename TPixelType>
void
SpatialObjectReader<TDimension,TPixelType>::ReportInvalidRequest()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::ReportInvalidRequest called...\n");
  this->InvokeEvent( ObjectInvalidRequestErrorEvent() );
}

template <unsigned int TDimension, typename TPixelType>
void
SpatialObjectReader<TDimension,TPixelType>::ReportObjectReadingError()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::ReportObjectReadingError: called...\n");
  this->InvokeEvent( ObjectReadingErrorEvent() );
}

template <unsigned int TDimension, typename TPixelType>
void
SpatialObjectReader<TDimension,TPixelType>::ReportObjectReadingSuccess()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::ReportObjectReadingSuccess: called...\n");
  this->InvokeEvent( ObjectReadingSuccessEvent() );
}

template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>
::RequestSetFileName( const FileNameType & filename )
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::RequestSetFileName called...\n");
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


template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>
::SetFileName()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::SetFileName called...\n");
  m_FileName = m_FileNameToBeSet;
}

/** Read the spatialobject file */
template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>::AttemptReadObject()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::AttemptReadObject called...\n");
  m_SpatialObjectReader->SetFileName( m_FileName.c_str() );

  try
    {
    m_SpatialObjectReader->Update();
    }
  catch( itk::ExceptionObject & )
    {
    this->m_StateMachine.PushInput( this->m_ObjectReadingErrorInput );
    return;
    }

  this->m_StateMachine.PushInput( this->m_ObjectReadingSuccessInput );
}


/** Request to read the object file */
template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>::RequestReadObject()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::RequestReadObject called...\n");
  this->m_StateMachine.PushInput( this->m_ReadObjectRequestInput);
  this->m_StateMachine.ProcessInputs();
}

/** Print Self function */
template <unsigned int TDimension, typename TPixelType>
void
SpatialObjectReader<TDimension,TPixelType>::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif
