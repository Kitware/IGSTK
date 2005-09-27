/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMImageReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkDICOMImageReader_txx
#define __igstkDICOMImageReader_txx

#include "igstkDICOMImageReader.h"
#include "igstkEvents.h"

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"

namespace igstk
{ 

/** Constructor */
template <class TPixelType>
DICOMImageReader<TPixelType>::DICOMImageReader() : m_StateMachine(this), m_Logger(NULL)
{ 

  //Set the state descriptors
  m_StateMachine.AddState(m_IdleState, "IdleState");
  m_StateMachine.AddState(m_ImageDirectoryNameReadState, "ImageDirectoryNameReadState");
  m_StateMachine.AddState(m_ImageReadState, "ImageReadState");

 /** List of State Inputs */

  m_StateMachine.AddInput(m_ReadImageRequestInput,"ImageReadRequestInput");

  m_StateMachine.AddInput(m_ImageDirectoryNameValidInput,"ImageDirectoryNameValidInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameIsEmptyInput,"ImageDirectoryNameIsEmptyInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameDoesNotExistInput,"ImageDirectoryNameDoesNotExistInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameIsNotDirectoryInput,"ImageDirectoryNameIsNotDirectoryInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameHasNotEnoughFilesInput,"m_ImageDirectoryNameHasNotEnoughFilesInput");

  m_StateMachine.AddTransition(m_IdleState,m_ImageDirectoryNameValidInput,m_ImageDirectoryNameReadState,&DICOMImageReader::SetDirectoryName);
  m_StateMachine.AddTransition(m_ImageDirectoryNameReadState,m_ReadImageRequestInput,m_ImageReadState,&DICOMImageReader::AttemptReadImage);
  m_StateMachine.AddTransition(m_IdleState,m_ReadImageRequestInput,m_IdleState,&DICOMImageReader::ReportInvalidRequest);

   // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Create the DICOM GDCM file reader
  m_FileNames = itk::GDCMSeriesFileNames::New();
  m_ImageIO = itk::GDCMImageIO::New();
  m_ImageSeriesReader = ImageSeriesReaderType::New();
} 

/** Destructor */
template <class TPixelType>
DICOMImageReader<TPixelType>::~DICOMImageReader()  
{

}

template <class TImageSpatialObject>
void DICOMImageReader<TImageSpatialObject>
::RequestSetDirectory( const DirectoryNameType & directory )
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestSetDirectory called...\n");

  m_ImageDirectoryNameToBeSet = directory;
 
  if( directory.empty() )
    {
    this->m_StateMachine.PushInput( this->m_ImageDirectoryNameIsEmptyInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  if( !itksys::SystemTools::FileExists( directory.c_str() ) )
    {
    this->m_StateMachine.PushInput( this->m_ImageDirectoryNameDoesNotExistInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  if( !itksys::SystemTools::FileIsDirectory( directory.c_str() ))
    {
    this->m_StateMachine.PushInput( this->m_ImageDirectoryNameIsNotDirectoryInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  // Attempt to load the directory listing.
  itksys::Directory directoryClass;
  directoryClass.Load( directory.c_str() );
  if( directoryClass.GetNumberOfFiles() < 3 ) // To count for  "." and ".." 
    {
    this->m_StateMachine.PushInput( this->m_ImageDirectoryNameHasNotEnoughFilesInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  this->m_StateMachine.PushInput( this->m_ImageDirectoryNameValidInput );
  this->m_StateMachine.ProcessInputs();
}


template <class TImageSpatialObject>
void 
DICOMImageReader<TImageSpatialObject>
::SetDirectoryName()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::SetDirectoryName called...\n");
  m_ImageDirectoryName = m_ImageDirectoryNameToBeSet;
  this->ReadDirectoryFileNames();
}


template <class TPixelType>
void DICOMImageReader<TPixelType>::RequestReadImage()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestReadImage called...\n");
  this->m_StateMachine.PushInput( this->m_ReadImageRequestInput);
  this->m_StateMachine.ProcessInputs();
}


/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::ReadDirectoryFileNames()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReadDirectoryFileNames called...\n");
  m_FileNames->SetInputDirectory( m_ImageDirectoryName );
  m_ImageSeriesReader->SetFileNames( m_FileNames->GetInputFileNames() );
}


/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::AttemptReadImage()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::AttemptReadImage called...\n");

  m_ImageSeriesReader->SetImageIO( m_ImageIO );
  
  try
    {
    m_ImageSeriesReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    this->InvokeEvent( DICOMImageReadingErrorEvent() );
    return;
    }
  
  m_ImageIO->GetPatientName(  m_PatientName  );
  m_ImageIO->GetPatientID(    m_PatientID    );
  m_ImageIO->GetModality(     m_Modality     );
}

/* The ReportInvalidRequest function reports invalid requests */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportInvalidRequest()
{
    igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportInvalidRequest called...\n");
    this->InvokeEvent( DICOMInvalidRequestErrorEvent() );
}



/** Get the DICOM modality */
template <class TPixelType>
const char *
DICOMImageReader<TPixelType>::GetModality() const
{
  return( m_Modality);
}


/** Get the patient name */
template <class TPixelType>
const char *
DICOMImageReader<TPixelType>::GetPatientName() const
{
  return( m_PatientName);
}


/** Get the image. This function MUST be private in order to  prevent unsafe
 * access to the ITK image level. */
template <class TPixelType>
const typename DICOMImageReader< TPixelType >::ImageType *
DICOMImageReader<TPixelType>::GetITKImage() const
{
  return m_ImageSeriesReader->GetOutput();
}


/** Print Self function */
template <class TPixelType>
void DICOMImageReader<TPixelType>::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif

