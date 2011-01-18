/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMImageReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
  igstkAddInputMacro( ReadImage );
  igstkAddInputMacro( ResetReader );
  igstkAddInputMacro( GetImage );
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
                           ReadImage,
                           AttemptingToReadImage,
                           AttemptReadImage );

  igstkAddTransitionMacro( AttemptingToReadImage,
                           ImageReadingSuccess,
                           ImageRead,
                           ReportImageReadingSuccess );

  //Transition for invalid inputs to Idle state
  igstkAddTransitionMacro( Idle,
                           ReadImage,
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

  igstkAddTransitionMacro( Idle,
                           GetImage,
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
                           GetImage,
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
                           GetImage,
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
                           ReadImage,
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
                           ReadImage,
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

  igstkAddTransitionMacro( AttemptingToReadImage,
                           GetImage,
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
                           GetImage,
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
  m_FileNames->SetRecursive(false);
  m_FileNames->SetGlobalWarningDisplay(this->GetGlobalWarningDisplay());

  // add more criteria to distinguish between different series
  m_FileNames->SetUseSeriesDetails( true );

  // add acquisition number
  // Commented out by Patrick Cheng,
  // Note: This is too restrictive, it fails to load a full 
  // series in some cases.
  // m_FileNames->AddSeriesRestriction( "0020|0012" );

  m_ImageIO = itk::GDCMImageIO::New();
  m_ImageIO->SetGlobalWarningDisplay(this->GetGlobalWarningDisplay());
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

  // To count for  "." and ".." and at least two dicom slices 
  if( directoryClass.GetNumberOfFiles() < 4 ) 
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
                 "igstk::DICOMImageReader::RequestGetImage called...\n" );
  this->m_StateMachine.PushInput( this->m_GetImageInput);
  this->m_StateMachine.ProcessInputs();
}

template <class TImageSpatialObject>
void 
DICOMImageReader<TImageSpatialObject>
::SetDirectoryNameProcessing()
{
  igstkLogMacro( DEBUG, 
                "igstk::DICOMImageReader::SetDirectoryName called...\n" );
  m_ImageDirectoryName = m_ImageDirectoryNameToBeSet;
  this->ReadDirectoryFileNamesProcessing();
}


template <class TPixelType>
void DICOMImageReader<TPixelType>::RequestReadImage()
{
  igstkLogMacro( DEBUG, 
                 "igstk::DICOMImageReader::RequestReadImage called...\n" );
  this->m_StateMachine.PushInput( this->m_ReadImageInput);
  this->m_StateMachine.ProcessInputs();
}

/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::ReadDirectoryFileNamesProcessing()
{
  igstkLogMacro( DEBUG, 
              "igstk::DICOMImageReader::ReadDirectoryFileNames called...\n" );
  
  m_FileNames->SetInputDirectory( m_ImageDirectoryName );
 
  const std::vector< std::string > & seriesUID = 
                                               m_FileNames -> GetSeriesUIDs();
   
  std::vector< std::string >::const_iterator iter = seriesUID.begin();
  
  for (; iter != seriesUID.end(); iter++)
    {
    igstkLogMacro( DEBUG, 
              "igstk::DICOMImageReader::seriesUID " << (*iter) << "\n");
    }
              
  if ( seriesUID.empty() ) 
    {
    this->m_ImageReadingErrorInformation = 
                                   "The dicom files are invalid or corrupted";
    this->m_StateMachine.PushInput( 
                            this->m_ImageSeriesFileNamesGeneratingErrorInput );
    this->m_StateMachine.ProcessInputs();
    return;
    } 
 
  // m_ImageSeriesReader->SetFileNames( m_FileNames->GetInputFileNames() );
 
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader will open seriesUID: " 
                                          << seriesUID.front().c_str() << "\n");
                            
  m_ImageSeriesReader->SetFileNames( m_FileNames->GetFileNames( 
                                                 seriesUID.front().c_str() )  );
  
  this->m_StateMachine.PushInput( 
                   this->m_ImageSeriesFileNamesGeneratingSuccessInput );
  this->m_StateMachine.ProcessInputs();
}


/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::AttemptReadImageProcessing()
{
  igstkLogMacro( DEBUG, 
                 "igstk::DICOMImageReader::AttemptReadImage called...\n" );

  try
    {
    m_ImageSeriesReader->UpdateLargestPossibleRegion();
    m_ImageSeriesReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    igstkLogMacro( DEBUG, 
    "igstk::DICOMImageReader - Failed to update image series reader.\n" );
    this->m_ImageReadingErrorInformation = excp.GetDescription();
    igstkLogMacro( DEBUG,"ITK Exception:"+ m_ImageReadingErrorInformation );
    this->m_StateMachine.PushInput( this->m_ImageReadingErrorInput );
    this->m_StateMachine.ProcessInputs();
    return;
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

    if( fabs(gantryTilt) > gantryTiltThreshold )
      {
      this->m_ImageReadingErrorInformation = "Unacceptable gantry tilt";
      this->m_StateMachine.PushInput( this->m_ImageReadingErrorInput );
      this->m_StateMachine.ProcessInputs();
      return; 
      }
    }
  else
    {
    igstkLogMacro( DEBUG, 
    "igstk::DICOMImageReader - Failed to retreive gantry info.\n" );
    }

  // Check if the dicom image data is being read by the correct DICOM image
  // reader derived class
  std::string modalityTagKey;
  
  modalityTagKey = "0008|0060";  

  if( itk::ExposeMetaData<std::string>(dict,modalityTagKey, m_Modality ) )
    {
    igstkLogMacro( DEBUG, "Modality     = " << m_Modality << "\n" );
    if( ! this->CheckModalityType( m_Modality ) )
      {
      this->m_ImageReadingErrorInformation = "Usage of a wrong image reader";
      this->m_StateMachine.PushInput( this->m_ImageReadingErrorInput );
      this->m_StateMachine.ProcessInputs();
      return; 
      }
    }
  else
    {
    igstkLogMacro( DEBUG, 
    "igstk::DICOMImageReader - Failed to retreive modality info.\n" );
    }

  this->m_StateMachine.PushInput( this->m_ImageReadingSuccessInput );
  this->m_StateMachine.ProcessInputs();

  std::string patientNameTagKey;
  patientNameTagKey = "0010|0010";  

  if( itk::ExposeMetaData<std::string>(dict,patientNameTagKey, m_PatientName ) )
    { 
    igstkLogMacro( DEBUG, "Patient Name = " << m_PatientName << "\n" );
    }
  else
    {
    igstkLogMacro( DEBUG, 
    "igstk::DICOMImageReader - Failed to retreive patient name.\n" );
    }
  
  std::string patientIDTagkey;
  patientIDTagkey = "0010|0020";  

  if( itk::ExposeMetaData<std::string>(dict,patientIDTagkey, m_PatientID ) )
    { 
    igstkLogMacro( DEBUG, "Patient ID = " << m_PatientID << "\n" );
    }
  else
    {
    igstkLogMacro( DEBUG, 
    "igstk::DICOMImageReader - Failed to retreive patient ID.\n" );
    }

  this->Superclass::ConnectImage();
  
  m_FileSuccessfullyRead = true;
}

/* This function reports invalid requests */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, 
      "igstk::DICOMImageReader::ReportInvalidRequestProcessing called...\n" );

  this->InvokeEvent( DICOMInvalidRequestErrorEvent() );
}

/* This function resets the reader */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::ResetReaderProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::ResetReader called...\n" );

  m_FileSuccessfullyRead = false;
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>::ReportImageDirectoryEmptyErrorProcessing()
{
  igstkLogMacro( DEBUG, 
     "igstk::DICOMImageReader::ReportImageDirectoryEmptyError called...\n" );

  this->InvokeEvent( DICOMImageDirectoryEmptyErrorEvent() );
}

template <class TPixelType>
void
DICOMImageReader<TPixelType>
::ReportImageDirectoryDoesNotExistErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::\
                    ReportImageDirectoryDoesNotExistError called...\n" );

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
         ReportImageDirectoryDoesNotHaveEnoughFilesError: called...\n" );

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
                 ReportImageDirectoryIsNotDirectoryError: called...\n" );

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
DICOMImageReader<TPixelType>::RequestGetModalityInformation() 
{
  igstkLogMacro( DEBUG,
        "igstk::DICOMImageReader::RequestGetModalityInformation called...\n");

  this->m_StateMachine.PushInput( this->m_GetModalityInformationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Request patient info */
template <class TPixelType>
void
DICOMImageReader<TPixelType>::RequestGetPatientNameInformation() 
{
  igstkLogMacro( DEBUG, 
    "igstk::DICOMImageReader::RequestGetPatientNameInformation called...\n");

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

/** Check modality type */
template <class TPixelType>
bool
DICOMImageReader<TPixelType>::CheckModalityType( DICOMInformationType modality )
{
  //Default modality type is CT
  if ( modality != "CT" )
    {
    return false;
    }
  else 
    {
    return true;
    }
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
