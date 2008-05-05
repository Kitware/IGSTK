/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

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
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ObjectFileNameRead );
  igstkAddStateMacro( ObjectRead );
  igstkAddStateMacro( ObjectAttemptingRead );

  // List of  Inputs
  igstkAddInputMacro( ReadObjectRequest );
  igstkAddInputMacro( ObjectReadingError );
  igstkAddInputMacro( ObjectReadingSuccess );
  igstkAddInputMacro( ObjectFileNameValid );
  igstkAddInputMacro( ObjectFileNameIsEmpty );
  igstkAddInputMacro( ObjectFileNameIsDirectory );
  igstkAddInputMacro( ObjectFileNameDoesNotExist );
  igstkAddInputMacro( GetOutput );

  // Transitions from Idle State
  igstkAddTransitionMacro( Idle, ObjectFileNameValid,
                           ObjectFileNameRead, SetFileName );
  igstkAddTransitionMacro( Idle, ObjectFileNameIsEmpty,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ObjectFileNameIsDirectory,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ObjectFileNameDoesNotExist,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ReadObjectRequest,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ObjectReadingError,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ObjectReadingSuccess,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, GetOutput,
                           Idle, ReportInvalidRequest );

  // Transitions from ObjectFileNameRead State
  igstkAddTransitionMacro( ObjectFileNameRead, ReadObjectRequest,
                           ObjectAttemptingRead, AttemptReadObject );
  igstkAddTransitionMacro( ObjectFileNameRead, ObjectReadingError,
                           ObjectFileNameRead, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectFileNameRead, ObjectReadingSuccess,
                           ObjectFileNameRead, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectFileNameRead, ObjectFileNameValid,
                           ObjectFileNameRead, SetFileName );
  igstkAddTransitionMacro( ObjectFileNameRead, ObjectFileNameIsEmpty,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectFileNameRead, ObjectFileNameIsDirectory,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectFileNameRead, ObjectFileNameDoesNotExist,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectFileNameRead, GetOutput,
                           ObjectFileNameRead, ReportInvalidRequest );

  // Transitions from ObjectRead State
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameValid,
                           ObjectFileNameRead, SetFileName );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameIsEmpty,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameIsDirectory,
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameDoesNotExist,
                           Idle, ReportInvalidRequest );
  // The following transition assumes that objects do not change after being
  // read the first time.
  igstkAddTransitionMacro( ObjectRead, ReadObjectRequest,
                           ObjectRead, ReportInvalidRequest ); 
  igstkAddTransitionMacro( ObjectRead, ObjectReadingSuccess,
                           ObjectRead, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectReadingError,
                           ObjectRead, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, GetOutput,
                           ObjectRead, ReportObject );


  // Transitions from ObjectAttemptingRead State
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectReadingError,
                           Idle, ReportObjectReadingError );
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectReadingSuccess,
                           ObjectRead, ReportObjectReadingSuccess );
  // if we are waiting, reading an object, we ignore new filenames 
  // regardless of whether they are valid or invalid (two next transitions).
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectFileNameValid,
                           ObjectAttemptingRead, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectFileNameIsEmpty,
                           ObjectAttemptingRead, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectFileNameIsDirectory,
                           ObjectAttemptingRead, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectFileNameDoesNotExist,
                           ObjectAttemptingRead, ReportInvalidRequest );
  // if we are waiting, reading an object, we ignore a redundant request for 
  // reading the object. We are already responding to a previous read request.
  igstkAddTransitionMacro( ObjectAttemptingRead, ReadObjectRequest,
                           ObjectAttemptingRead, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectAttemptingRead, GetOutput,
                           ObjectAttemptingRead, ReportInvalidRequest );


  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

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
SpatialObjectReader<TDimension,TPixelType>
::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                        ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent( ObjectInvalidRequestErrorEvent() );
}

template <unsigned int TDimension, typename TPixelType>
void
SpatialObjectReader<TDimension,TPixelType>
::ReportObjectReadingErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                        ReportObjectReadingErrorProcessing: called...\n");
  this->InvokeEvent( ObjectReadingErrorEvent() );
}

template <unsigned int TDimension, typename TPixelType>
void
SpatialObjectReader<TDimension,TPixelType>
::ReportObjectReadingSuccessProcessing()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                        ReportObjectReadingSuccessProcessing: called...\n");
  this->InvokeEvent( ObjectReadingSuccessEvent() );
}

template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>
::RequestSetFileName( const FileNameType & filename )
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                        RequestSetFileName called...\n");
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
::SetFileNameProcessing()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                        SetFileNameProcessing called...\n");
  m_FileName = m_FileNameToBeSet;
}

/** Read the spatialobject file */
template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>
::AttemptReadObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                        AttemptReadObjectProcessing called...\n");
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

/** Read the spatialobject file */
template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>
::ReportObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                                ReportObjectProcessing called...\n");
}

/** Request to read the object file */
template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>
::RequestReadObject()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                        RequestReadObject called...\n");
  this->m_StateMachine.PushInput( this->m_ReadObjectRequestInput);
  this->m_StateMachine.ProcessInputs();
}

/** Request to read the object file */
template <unsigned int TDimension, typename TPixelType>
void SpatialObjectReader<TDimension,TPixelType>
::RequestGetOutput()
{
  igstkLogMacro( DEBUG, "igstk::SpatialObjectReader::\
                                RequestGetOutput called...\n");
  this->m_StateMachine.PushInput( this->m_GetOutputInput);
  this->m_StateMachine.ProcessInputs();
}

/** Print Self function */
template <unsigned int TDimension, typename TPixelType>
void
SpatialObjectReader<TDimension,TPixelType>
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif
