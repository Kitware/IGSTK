/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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

  /** List of  Inputs */
  igstkAddInputMacro( ReadObjectRequest );
  igstkAddInputMacro( ObjectReadingError );
  igstkAddInputMacro( ObjectReadingSuccess );
  igstkAddInputMacro( ObjectFileNameValid );
  igstkAddInputMacro( ObjectFileNameIsEmpty );
  igstkAddInputMacro( ObjectFileNameIsDirectory );
  igstkAddInputMacro( ObjectFileNameDoesNotExist );

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
  igstkAddTransitionMacro( ObjectFileNameRead, ReadObjectRequest, 
                           ObjectAttemptingRead, AttemptReadObject );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameValid, 
                           ObjectFileNameRead, SetFileName );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameIsEmpty, 
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameIsDirectory, 
                           Idle, ReportInvalidRequest );
  igstkAddTransitionMacro( ObjectRead, ObjectFileNameDoesNotExist, 
                           Idle, ReportInvalidRequest );


  //Errors related to Object reading 
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectReadingError, 
                           Idle, ReportObjectReadingError );
  igstkAddTransitionMacro( ObjectAttemptingRead, ObjectReadingSuccess, 
                           ObjectRead, ReportObjectReadingSuccess );

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
