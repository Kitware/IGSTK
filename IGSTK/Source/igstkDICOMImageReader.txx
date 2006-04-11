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
  igstkAddStateMacro( ImageSeriesFileNamesGenerated ); 
  igstkAddStateMacro( ImageRead ); 
  igstkAddStateMacro( AttemptingToReadImage ); 

  /** List of  Inputs */
  igstkAddInputMacro( GetModalityInformation );
  igstkAddInputMacro( GetPatientNameInformation );
  igstkAddInputMacro( ReadImageRequest );
  igstkAddInputMacro( ResetReader );
  igstkAddInputMacro( RequestImage );
  igstkAddInputMacro( ImageReadingError );
  igstkAddInputMacro( ImageReadingSuccess );
  igstkAddInputMacro( ImageDirectoryNameValid );
  igstkAddInputMacro( ImageSeriesFileNamesGeneratingSuccess );
  igstkAddInputMacro( ImageSeriesFileNamesGeneratingError );
  igstkAddInputMacro( ImageDirectoryNameIsEmpty );
  igstkAddInputMacro( ImageDirectoryNameDoesNotExist );
  igstkAddInputMacro( ImageDirectoryNameIsNotDirectory );
  igstkAddInputMacro( ImageDirectoryNameDoesNotHaveEnoughFiles );

  //Transitions for valid directory name
  igstkAddTransitionMacro( Idle,
                           ImageDirectoryNameValid,
                           ImageDirectoryNameRead,
                           SetDirectoryName );
  
  //Transitions for image series file name reading 
  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageSeriesFileNamesGeneratingError,
                           Idle,
                           ReportImageSeriesFileNamesGeneratingError );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageSeriesFileNamesGeneratingSuccess,
                           ImageSeriesFileNamesGenerated,
                           ReportImageSeriesFileNamesGeneratingSuccess );

  // Transitions for image reading
  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ReadImageRequest,
                           AttemptingToReadImage,
                           AttemptReadImage );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageReadingSuccess,
                           ImageRead,
                           ReportImageReadingSuccess );

  //Transition for invalid inputs to Idle state
  igstkAddTransitionMacro( Idle,
                           ReadImageRequest,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           GetModalityInformation,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           GetPatientNameInformation,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           ImageReadingError,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           ImageReadingSuccess,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           ImageSeriesFileNamesGeneratingSuccess,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           ImageSeriesFileNamesGeneratingError,
                           Idle,
                           ReportInvalidRequest );

  // Transitions for Invalid inputs to  ImageDirectoryNameRead state 
  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           GetModalityInformation,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           GetPatientNameInformation,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageReadingSuccess,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageReadingError,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageDirectoryNameValid,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageSeriesFileNamesGeneratingError,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageSeriesFileNamesGeneratingSuccess,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageDirectoryNameIsEmpty,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageDirectoryNameIsNotDirectory,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageDirectoryNameRead,
                           ImageDirectoryNameDoesNotHaveEnoughFiles,
                           ImageDirectoryNameRead,
                           ReportInvalidRequest );

  // Transitions for invalid inputs to ImageSeriesFileNamesGenerated 

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           GetModalityInformation,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           GetPatientNameInformation,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageReadingSuccess,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageReadingError,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageDirectoryNameValid,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageSeriesFileNamesGeneratingSuccess,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageSeriesFileNamesGeneratingError,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageDirectoryNameIsEmpty,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageDirectoryNameDoesNotExist,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageDirectoryNameIsNotDirectory,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageSeriesFileNamesGenerated,
                           ImageDirectoryNameDoesNotHaveEnoughFiles,
                           ImageSeriesFileNamesGenerated,
                           ReportInvalidRequest );

  // Transitions for invalid inputs to ImageRead state
  igstkAddTransitionMacro( ImageRead,
                           ReadImageRequest,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageReadingSuccess,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageReadingError,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageDirectoryNameValid,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageSeriesFileNamesGeneratingSuccess,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageSeriesFileNamesGeneratingError,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageDirectoryNameIsEmpty,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageDirectoryNameDoesNotExist,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageDirectoryNameIsNotDirectory,
                           ImageRead,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageRead,
                           ImageDirectoryNameDoesNotHaveEnoughFiles,
                           ImageRead,
                           ReportInvalidRequest );


  // Transitions for invalid inputs to AttemptingToReadImage state
  igstkAddTransitionMacro( AttemptingToReadImage,
                           ReadImageRequest,
                           AttemptingToReadImage,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageDirectoryNameValid,
                           AttemptingToReadImage,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageSeriesFileNamesGeneratingSuccess,
                           AttemptingToReadImage,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageSeriesFileNamesGeneratingError,
                           AttemptingToReadImage,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageDirectoryNameIsEmpty,
                           AttemptingToReadImage,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageDirectoryNameDoesNotExist,
                           AttemptingToReadImage,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageDirectoryNameIsNotDirectory,
                           AttemptingToReadImage,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageDirectoryNameDoesNotHaveEnoughFiles,
                           AttemptingToReadImage,
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

  igstkAddTransitionMacro( Idle,
                           ResetReader,
                           Idle,
                           ResetReader );

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


  igstkAddTransitionMacro( ImageRead,
                           RequestImage,
                           ImageRead,
                           ReportImage );


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

  // In the case we only have one file we create a standard file writer
  m_ImageFileReader = ImageReaderType::New();
  m_ImageFileReader->SetImageIO( m_ImageIO );

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
  igstkLogMacro( DEBUG, 
                 "igstk::DICOMImageReader::RequestSetDirectory called...\n");


  // Reset the reader 
  this->m_StateMachine.PushInput( this->m_ResetReaderInput );
  this->m_StateMachine.ProcessInputs();

  m_ImageDirectoryNameToBeSet = directory;
 
  if( directory.empty() )
    {
    this->m_StateMachine.PushInput( this->m_ImageDirectoryNameIsEmptyInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  if( !itksys::SystemTools::FileExists( directory.c_str() ) )
    {
    this->m_StateMachine.PushInput( 
                               this->m_ImageDirectoryNameDoesNotExistInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  if( !itksys::SystemTools::FileIsDirectory(  directory.c_str() ))
    {
    this->m_StateMachine.PushInput( 
                              this->m_ImageDirectoryNameIsNotDirectoryInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  // Attempt to load the directory listing.
  itksys::Directory directoryClass;
  directoryClass.Load( directory.c_str() );
  if( directoryClass.GetNumberOfFiles() < 3 ) // To count for  "." and ".." 
    {
    this->m_StateMachine.PushInput( 
                      this->m_ImageDirectoryNameDoesNotHaveEnoughFilesInput );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  this->m_StateMachine.PushInput( this->m_ImageDirectoryNameValidInput );
  this->m_StateMachine.ProcessInputs();

}

/** This function reports the image */
template <class TImageSpatialObject>
void 
DICOMImageReader<TImageSpatialObject>
::ReportImageProcessing()
{
  ImageModifiedEvent  event;
  event.Set( this->m_ImageSpatialObject );
  this->InvokeEvent( event );
}

template <class TPixelType>
void DICOMImageReader<TPixelType>::RequestGetImage()
{
  igstkLogMacro( DEBUG, 
                 "igstk::DICOMImageReader::RequestGetImage called...\n");
  this->m_StateMachine.PushInput( this->m_RequestImageInput);
  this->m_StateMachine.ProcessInputs();
}

template <class TImageSpatialObject>
void 
DICOMImageReader<TImageSpatialObject>
::SetDirectoryNameProcessing()
{
  igstkLogMacro( DEBUG, 
                "igstk::DICOMImageReader::SetDirectoryName called...\n");
  m_ImageDirectoryName = m_ImageDirectoryNameToBeSet;
  this->ReadDirectoryFileNamesProcessing();
}


template <class TPixelType>
void DICOMImageReader<TPixelType>::RequestReadImage()
{
  igstkLogMacro( DEBUG, 
                 "igstk::DICOMImageReader::RequestReadImage called...\n");
  this->m_StateMachine.PushInput( this->m_ReadImageRequestInput);
  this->m_StateMachine.ProcessInputs();
}

/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::ReadDirectoryFileNamesProcessing()
{
  igstkLogMacro( DEBUG, 
              "igstk::DICOMImageReader::ReadDirectoryFileNames called...\n");
  
  m_FileNames->SetInputDirectory( m_ImageDirectoryName );
 
  const std::vector< std:: string > & seriesUID = 
                                               m_FileNames -> GetSeriesUIDs();
  if ( seriesUID.empty() ) 
    {
    this->m_ImageReadingErrorInformation = 
                                   "The dicom files are invalid or corrupted";
    this->m_StateMachine.PushInput( 
                            this->m_ImageSeriesFileNamesGeneratingErrorInput );
    this->m_StateMachine.ProcessInputs();
    return;
    } 
 
  m_ImageSeriesReader->SetFileNames( m_FileNames->GetInputFileNames() );
  this->m_StateMachine.PushInput( 
                   this->m_ImageSeriesFileNamesGeneratingSuccessInput );
  this->m_StateMachine.ProcessInputs();
}


/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::AttemptReadImageProcessing()
{
  igstkLogMacro( DEBUG, 
                 "igstk::DICOMImageReader::AttemptReadImage called...\n");

  if(m_ImageSeriesReader->GetFileNames().size()>1)
    {
    try
      {
      m_ImageSeriesReader->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      this->m_ImageReadingErrorInformation = excp.GetDescription();
      this->m_StateMachine.PushInput( this->m_ImageReadingErrorInput );
      this->m_StateMachine.ProcessInputs();
      return;
      }
    }
  else
    {
    try
      {
      m_ImageFileReader->SetFileName(
        m_ImageSeriesReader->GetFileNames()[0].c_str());
      m_ImageFileReader->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      this->m_ImageReadingErrorInformation = excp.GetDescription();
      this->m_StateMachine.PushInput( this->m_ImageReadingErrorInput );
      this->m_StateMachine.ProcessInputs();
      return;
      }
    }

  // Check if the DICOM image has a gantry tilt or not 
 
  std::string tagkey;

  itk::MetaDataDictionary & dict = m_ImageIO->GetMetaDataDictionary();
 
  tagkey = "0018|1120";

  if( itk::ExposeMetaData<std::string>(dict,tagkey, m_GantryTilt ) )
    {
    igstkLogMacro( DEBUG, "Gantry Tilt = " << m_GantryTilt << "\n" );

    // Check if the gantry tilt is within an acceptable range
    double gantryTilt;
    double gantryTiltThreshold=0.01;
  
    std::stringstream sstr;
    sstr.str( m_GantryTilt );
    sstr >> gantryTilt;

    if( gantryTilt > gantryTiltThreshold )
      {
      this->m_ImageReadingErrorInformation = "Unacceptable gantry tilt";
      this->m_StateMachine.PushInput( this->m_ImageReadingErrorInput );
      this->m_StateMachine.ProcessInputs();
      return; 
      }
    }

  this->m_StateMachine.PushInput( this->m_ImageReadingSuccessInput );
  this->m_StateMachine.ProcessInputs();

  char tmp_string[5120];

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
  
  m_FileSuccessfullyRead = true;
}

/* This function reports invalid requests */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, 
      "igstk::DICOMImageReader::ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent( DICOMInvalidRequestErrorEvent() );
}

/* This function resets the reader */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ResetReaderProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ResetReader called...\n");
  m_FileSuccessfullyRead = false;
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryEmptyErrorProcessing()
{
  igstkLogMacro( DEBUG, 
     "igstk::DICOMImageReader::ReportImageDirectoryEmptyError called...\n");
  this->InvokeEvent( DICOMImageDirectoryEmptyErrorEvent() );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>
::ReportImageDirectoryDoesNotExistErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::\
                    ReportImageDirectoryDoesNotExistError called...\n");
  DICOMImageDirectoryDoesNotExistErrorEvent event;
  event.Set( m_ImageDirectoryNameToBeSet );
  this->InvokeEvent( event );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>
::ReportImageDirectoryDoesNotHaveEnoughFilesErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::\
         ReportImageDirectoryDoesNotHaveEnoughFilesError: called...\n");
  DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent event;
  event.Set( m_ImageDirectoryNameToBeSet );
  this->InvokeEvent( event );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>
::ReportImageDirectoryIsNotDirectoryErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::\
                 ReportImageDirectoryIsNotDirectoryError: called...\n");
  DICOMImageDirectoryIsNotDirectoryErrorEvent event;
  event.Set( m_ImageDirectoryNameToBeSet );
  this->InvokeEvent( event );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>
::ReportImageSeriesFileNamesGeneratingErrorProcessing()
{
  igstkLogMacro( DEBUG, 
         "igstk::DICOMImageReader::\
         ReportImageSeriesFilesNamesGeneratingErrorProcessing: called...\n");

  DICOMImageSeriesFileNamesGeneratingErrorEvent event;
  this->InvokeEvent( event );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>
::ReportImageSeriesFileNamesGeneratingSuccessProcessing()
{
  igstkLogMacro( DEBUG, 
         "igstk::DICOMImageReader::\
         ReportImageSeriesFileNamesGeneratingSuccessProcessing: called...\n");
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageReadingErrorProcessing()
{
  igstkLogMacro( DEBUG, 
            "igstk::DICOMImageReader::ReportImageReadingError: called...\n");

  DICOMImageReadingErrorEvent event;
  event.Set ( this->m_ImageReadingErrorInformation );
  this->InvokeEvent( event );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageReadingSuccessProcessing()
{
  igstkLogMacro( DEBUG, 
  "igstk::DICOMImageReader::ReportImageReadingSuccessProcessing: called...\n");
}


/** Request the DICOM modality info */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::RequestModalityInformation() 
{
  igstkLogMacro( DEBUG,
            "igstk::DICOMImageReader::RequestModalityInformation called...\n");
  this->m_StateMachine.PushInput( this->m_GetModalityInformationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Request patient info */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::RequestPatientNameInformation() 
{
  igstkLogMacro( DEBUG, 
    "igstk::DICOMImageReader::RequestPatientNameInformation called...\n");
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
  if(m_ImageSeriesReader->GetFileNames().size()>1)
    {
    return m_ImageSeriesReader->GetOutput();
    }
  return m_ImageFileReader->GetOutput();
}


/** Print Self function */
template <class TPixelType>
void DICOMImageReader<TPixelType>
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
}

} // end namespace igstk

#endif
