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

  m_StateMachine.AddInput(m_GetModalityInfoInput,"GetModalityInfoInput");
  m_StateMachine.AddInput(m_GetPatientNameInfoInput,"GetPatientNameInfoInput");
  m_StateMachine.AddInput(m_ReadImageRequestInput,"ImageReadRequestInput");
  m_StateMachine.AddInput(m_ImageReadingErrorInput,"ImageReadingErrorInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameValidInput,"ImageDirectoryNameValidInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameIsEmptyInput,"ImageDirectoryNameIsEmptyInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameDoesNotExistInput,"ImageDirectoryNameDoesNotExistInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameIsNotDirectoryInput,"ImageDirectoryNameIsNotDirectoryInput");
  m_StateMachine.AddInput(m_ImageDirectoryNameDoesNotHaveEnoughFilesInput,"m_ImageDirectoryNameDoesNotHaveEnoughFilesInput");

  m_StateMachine.AddTransition(m_IdleState,m_ImageDirectoryNameValidInput,m_ImageDirectoryNameReadState,&DICOMImageReader::SetDirectoryName);
  m_StateMachine.AddTransition(m_ImageDirectoryNameReadState,m_ReadImageRequestInput,m_ImageReadState,&DICOMImageReader::AttemptReadImage);
  m_StateMachine.AddTransition(m_IdleState,m_ReadImageRequestInput,m_IdleState,&DICOMImageReader::ReportInvalidRequest);

  //Transitions for DICOM info request input 
  m_StateMachine.AddTransition(m_ImageReadState,m_GetModalityInfoInput,m_ImageReadState,&DICOMImageReader::GetModalityInfo);
  m_StateMachine.AddTransition(m_ImageReadState,m_GetPatientNameInfoInput,m_ImageReadState,&DICOMImageReader::GetPatientNameInfo);

  //Errors related to image directory name
  m_StateMachine.AddTransition(m_IdleState,m_ImageDirectoryNameIsEmptyInput,m_IdleState,&DICOMImageReader::ReportImageDirectoryEmptyError);
  m_StateMachine.AddTransition(m_IdleState,m_ImageDirectoryNameDoesNotExistInput,m_IdleState,&DICOMImageReader::ReportImageDirectoryDoesNotExistError);
  m_StateMachine.AddTransition(m_IdleState,m_ImageDirectoryNameIsNotDirectoryInput,m_IdleState,&DICOMImageReader::ReportImageDirectoryIsNotDirectoryError);
  m_StateMachine.AddTransition(m_IdleState,m_ImageDirectoryNameDoesNotHaveEnoughFilesInput,m_IdleState,&DICOMImageReader::ReportImageDirectoryDoesNotHaveEnoughFilesError);

  //Errors related to  image reading 
  m_StateMachine.AddTransition(m_ImageDirectoryNameReadState,m_ImageReadingErrorInput,m_ImageDirectoryNameReadState,&DICOMImageReader::ReportImageReadingError);

   // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Create the DICOM GDCM file reader
  m_FileNames = itk::GDCMSeriesFileNames::New();
  m_ImageIO = itk::GDCMImageIO::New();
  m_ImageSeriesReader = ImageSeriesReaderType::New();

  m_ImageSeriesReader->SetImageIO( m_ImageIO );
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
    this->m_StateMachine.PushInput( this->m_ImageDirectoryNameDoesNotHaveEnoughFilesInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  this->m_StateMachine.PushInput( this->m_ImageDirectoryNameValidInput );
  this->m_StateMachine.ProcessInputs();

  m_ImageDirectoryNameToBeSet = directory;
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

  try
    {
    m_ImageSeriesReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    this->InvokeEvent( DICOMImageReadingErrorEvent() );
    return;
    }
  
  m_ImageIO->GetPatientName(  tmp_string  );
  m_PatientName = tmp_string;
  
  m_ImageIO->GetPatientID(  tmp_string );
  m_PatientID  = tmp_string;

  m_ImageIO->GetModality(   tmp_string );
  m_Modality  = tmp_string;


  igstkLogMacro( DEBUG, "Patient Name = " << m_PatientName << "\n" );
  igstkLogMacro( DEBUG, "Patient ID   = " << m_PatientID << "\n" );
  igstkLogMacro( DEBUG, "Modality     = " << m_Modality << "\n" );

  this->Superclass::ConnectImage();
}

/* This function reports invalid requests */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportInvalidRequest()
{
    igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportInvalidRequest called...\n");
    this->InvokeEvent( DICOMInvalidRequestErrorEvent() );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryEmptyError()
{
    igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageDirectoryEmptyError called...\n");
    this->InvokeEvent( DICOMImageDirectoryEmptyErrorEvent());
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryDoesNotExistError()
{
    igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageDirectoryDoesNotExistError called...\n");
    this->InvokeEvent( DICOMImageDirectoryDoesNotExistErrorEvent());
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryDoesNotHaveEnoughFilesError()
{
    igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageDirectoryDoesNotHaveEnoughFilesError: called...\n");
    this->InvokeEvent( DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent() );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryIsNotDirectoryError()
{
    igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageDirectoryIsNotDirectoryError: called...\n");
    this->InvokeEvent( DICOMImageDirectoryIsNotDirectoryErrorEvent() );
}


template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageReadingError()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageReadingError: called...\n");
  this->InvokeEvent( DICOMImageReadingErrorEvent() );
}


/** Request the DICOM modality info */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::RequestModalityInfo() 
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestModalityInfo called...\n");
  this->m_StateMachine.PushInput( this->m_GetModalityInfoInput);
  this->m_StateMachine.ProcessInputs();
}

/** Request patient info */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::RequestPatientNameInfo() 
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestPatientNameInfo called...\n");
  this->m_StateMachine.PushInput( this->m_GetPatientNameInfoInput);
  this->m_StateMachine.ProcessInputs();
}

/** Get the DICOM modality */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::GetModalityInfo() 
{
  DICOMModalityEvent event;
  event.SetString(m_Modality);
  this->InvokeEvent(event);
}

/** Get the patient name */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::GetPatientNameInfo() 
{
  DICOMPatientNameEvent event;
  event.SetString(m_PatientName);
  this->InvokeEvent(event);
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

