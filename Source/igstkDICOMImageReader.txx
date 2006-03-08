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

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"

namespace igstk
{ 

/** Constructor */
template <class TPixelType>
DICOMImageReader<TPixelType>::DICOMImageReader() : m_StateMachine(this)
{ 
  m_Logger = NULL;
 
  //Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ImageDirectoryNameRead ); 
  igstkAddStateMacro( ImageRead ); 
  igstkAddStateMacro( AttemptingToReadImage ); 

 /** List of  Inputs */
  igstkAddInputMacro( GetModalityInformation );
  igstkAddInputMacro( GetPatientNameInformation );
  igstkAddInputMacro( ReadImageRequest );
  igstkAddInputMacro( ResetReader );
  igstkAddInputMacro( ImageReadingError );
  igstkAddInputMacro( ImageReadingSuccess );
  igstkAddInputMacro( ImageDirectoryNameValid );
  igstkAddInputMacro( ImageDirectoryNameIsEmpty );
  igstkAddInputMacro( ImageDirectoryNameDoesNotExist );
  igstkAddInputMacro( ImageDirectoryNameIsNotDirectory );
  igstkAddInputMacro( ImageDirectoryNameDoesNotHaveEnoughFiles );

  //Transitions for valid directory name
  igstkAddTransitionMacro( Idle,
                           ImageDirectoryNameValid,
                           ImageDirectoryNameRead,
                           SetDirectoryName );

  //Transition for valid image read request
  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ReadImageRequest,
                           AttemptingToReadImage,
                           AttemptReadImage );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageReadingSuccess,
                           ImageRead,
                           ReportImageReadingSuccess );

  //Transition for invalid image reqes request
  igstkAddTransitionMacro( Idle,
                           ReadImageRequest,
                           Idle,
                           ReportInvalidRequest );

  //Transitions for DICOM info request inputs
  igstkAddTransitionMacro( ImageRead,
                           GetModalityInformation,
                           ImageRead,
                           GetModalityInformation );

  igstkAddTransitionMacro( ImageRead,
                           GetPatientNameInformation,
                           ImageRead,
                           GetPatientNameInformation );

  //Transitions for reset reader input 

  igstkAddTransitionMacro( ImageRead,
                           ResetReader,
                           Idle,
                           ResetReader );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ResetReader,
                           Idle,
                           ResetReader );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ResetReader,
                           Idle,
                           ResetReader );


  //Errors related to image directory name
  igstkAddTransitionMacro( Idle,
                           ImageDirectoryNameIsEmpty,
                           Idle,
                           ReportImageDirectoryEmptyError );

  igstkAddTransitionMacro( Idle,
                           ImageDirectoryNameDoesNotExist,
                           Idle,
                           ReportImageDirectoryDoesNotExistError );

  igstkAddTransitionMacro( Idle,
                           ImageDirectoryNameIsNotDirectory,
                           Idle,
                           ReportImageDirectoryIsNotDirectoryError );

  igstkAddTransitionMacro( Idle,
                           ImageDirectoryNameDoesNotHaveEnoughFiles,
                           Idle,
                           ReportImageDirectoryDoesNotHaveEnoughFilesError );

  //Errors related to  image reading 
  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageReadingError,
                           Idle,
                           ReportImageReadingError );

   // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Initialize the booleas for the preconditions of the unsafe Get macros 
  m_FileSuccessfullyRead = false;

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

}


template <class TImageSpatialObject>
void 
DICOMImageReader<TImageSpatialObject>
::SetDirectoryNameProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::SetDirectoryName called...\n");
  m_ImageDirectoryName = m_ImageDirectoryNameToBeSet;
  this->ReadDirectoryFileNamesProcessing();
}


template <class TPixelType>
void DICOMImageReader<TPixelType>::RequestReadImage()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestReadImage called...\n");
  this->m_StateMachine.PushInput( this->m_ReadImageRequestInput);
  this->m_StateMachine.ProcessInputs();
}

template <class TPixelType>
void DICOMImageReader<TPixelType>::RequestResetReader()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestResetReader called...\n");
  this->m_StateMachine.PushInput( this->m_ResetReaderInput);
  this->m_StateMachine.ProcessInputs();
}

/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::ReadDirectoryFileNamesProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReadDirectoryFileNames called...\n");
  
  m_FileNames->SetInputDirectory( m_ImageDirectoryName );
 
  const std::vector< std:: string > & seriesUID = m_FileNames -> GetSeriesUIDs();
  if ( seriesUID.empty() ) 
  {
    DICOMImageReadingErrorEvent event;
    this->InvokeEvent ( event );
    return;
  } 
 
  m_ImageSeriesReader->SetFileNames( m_FileNames->GetInputFileNames() );
}


/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::AttemptReadImageProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::AttemptReadImage called...\n");

  try
    {
    m_ImageSeriesReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    this->m_StateMachine.PushInput( this->m_ImageReadingErrorInput );    
    DICOMImageReadingErrorEvent event;
    event.Set( excp.GetDescription() );
    this->InvokeEvent( event );
    return;
    }

  this->m_StateMachine.PushInput( this->m_ImageReadingSuccessInput );    

  this->m_StateMachine.ProcessInputs();

  std::string tagkey;
    
  itk::MetaDataDictionary & dict = m_ImageIO->GetMetaDataDictionary();

  // 
  // Get the Patient Name from the DICOM header
  //
  tagkey = "0010|0010";

  if( itk::ExposeMetaData<std::string>(dict,tagkey, m_PatientName ) )
    {
    igstkLogMacro( DEBUG, "Patient Name = " << m_PatientName << "\n" );
    }
  else
    {
    igstkLogMacro( CRITICAL, "Patient Name not found in DICOM file \n" );
    return;
    }

  // 
  // Get the Patient ID from the DICOM header
  //
  tagkey = "0010|0020";

  if( itk::ExposeMetaData<std::string>(dict,tagkey, m_PatientID ) )
    {
    igstkLogMacro( DEBUG, "Patient ID = " << m_PatientID << "\n" );
    }
  else
    {
    igstkLogMacro( CRITICAL, "Patient ID not found in DICOM file \n" );
    return;
    }


  // 
  // Get the Modality from the DICOM header
  //
  tagkey = "0008|0060";

  if( itk::ExposeMetaData<std::string>(dict,tagkey, m_Modality ) )
    {
    igstkLogMacro( DEBUG, "Modality     = " << m_Modality << "\n" );
    }
  else
    {
    igstkLogMacro( CRITICAL, "Image Modality not found in DICOM file \n" );
    return;
    }

  this->Superclass::ConnectImage();
  
  m_FileSuccessfullyRead = true;
}

/* This function reports invalid requests */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent( DICOMInvalidRequestErrorEvent() );
}

/* This function resets the reader */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ResetReaderProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ResetReader called...\n");
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryEmptyErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageDirectoryEmptyError called...\n");
  this->InvokeEvent( DICOMImageDirectoryEmptyErrorEvent() );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryDoesNotExistErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageDirectoryDoesNotExistError called...\n");
  DICOMImageDirectoryDoesNotExistErrorEvent event;
  event.Set( m_ImageDirectoryNameToBeSet );
  this->InvokeEvent( event );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryDoesNotHaveEnoughFilesErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageDirectoryDoesNotHaveEnoughFilesError: called...\n");
  DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent event;
  event.Set( m_ImageDirectoryNameToBeSet );
  this->InvokeEvent( event );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryIsNotDirectoryErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageDirectoryIsNotDirectoryError: called...\n");
  DICOMImageDirectoryIsNotDirectoryErrorEvent event;
  event.Set( m_ImageDirectoryNameToBeSet );
  this->InvokeEvent( event );
}


template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageReadingErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageReadingError: called...\n");
  this->InvokeEvent( DICOMImageReadingErrorEvent() );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageReadingSuccessProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ReportImageReadingSuccessProcessing: called...\n");
}


/** Request the DICOM modality info */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::RequestModalityInformation() 
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestModalityInformation called...\n");
  this->m_StateMachine.PushInput( this->m_GetModalityInformationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Request patient info */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::RequestPatientNameInformation() 
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::RequestPatientNameInformation called...\n");
  this->m_StateMachine.PushInput( this->m_GetPatientNameInformationInput);
  this->m_StateMachine.ProcessInputs();
}

/** Get the DICOM modality */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::GetModalityInformationProcessing() 
{
  DICOMModalityEvent event;
  event.Set( m_Modality );
  this->InvokeEvent( event );
}

/** Get the patient name */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::GetPatientNameInformationProcessing() 
{
  DICOMPatientNameEvent event;
  event.Set( m_PatientName );
  this->InvokeEvent( event );
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
  Superclass::PrintSelf( os, indent );
}

} // end namespace igstk

#endif

