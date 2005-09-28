/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMImageReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkDICOMImageReader_h
#define __igstkDICOMImageReader_h

#include "igstkMacros.h"
#include "igstkImageReader.h"

#include "igstkStateMachine.h"
#include "itkLogger.h"

#include "itkImageSeriesReader.h"
#include "itkObject.h"
#include "itkEventObject.h"

#include "igstkEvents.h"
#include "igstkStringEvents.h"

#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"


namespace igstk
{

/** \class DICOMImageReader
 * 
 * \brief This class reads DICOM files. This class should not be instantiated
 * directly, instead the derived classes that are specific to particular image
 * modalities should be used.
 * 
 * \ingroup Readers
 */

template <class TImageSpatialObject>
class DICOMImageReader : public ImageReader< TImageSpatialObject >
{

public:

  /** Typedefs */
  typedef DICOMImageReader                             Self;
  typedef ImageReader< TImageSpatialObject >           Superclass;
  typedef itk::SmartPointer< Self >                    Pointer;
  typedef itk::SmartPointer< const Self >              ConstPointer;
  typedef typename Superclass::ImageType               ImageType;
  typedef typename ImageType::ConstPointer             ImageConstPointer;
  typedef itk::Logger                                  LoggerType;

  typedef itk::ImageSeriesReader< ImageType >          ImageSeriesReaderType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( DICOMImageReader, ImageReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  typedef std::string    DirectoryNameType;
  typedef std::string    DICOMInfoType;
  
  /** Method to pass the directory name containing the DICOM image data */
  void RequestSetDirectory( const DirectoryNameType & directory );

  /** This method request image read **/
  void RequestReadImage();

  /** This function should be used to request modality info*/
  void RequestModalityInfo();

  /** This function will be used to request patient name info */
  void RequestPatientNameInfo();
  
  /** Declarations needed for the State Machine */
  igstkStateMachineTemplatedMacro();

  /** Declarations needed for the Logger */
  igstkLoggerMacro();

  itkEventMacro( DICOMImageReaderEvent,                             IGSTKEvent);
  itkEventMacro( DICOMModalityEvent,                                StringEvents);
  itkEventMacro( DICOMPatientNameEvent,                             StringEvents);
 

  // Invalid request error event 
  itkEventMacro( DICOMInvalidRequestErrorEvent,                      DICOMImageReaderEvent );
  
  // Events to handle errors with the ImageDirectory name
  itkEventMacro(DICOMImageDirectoryEmptyErrorEvent,                  DICOMImageReaderEvent );
  itkEventMacro(DICOMImageDirectoryDoesNotExistErrorEvent,           DICOMImageReaderEvent );
  itkEventMacro(DICOMImageDirectoryIsNotDirectoryErrorEvent,         DICOMImageReaderEvent );
  itkEventMacro(DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent, DICOMImageReaderEvent );

  //Image reading error
  itkEventMacro( DICOMImageReadingErrorEvent,                        DICOMImageReaderEvent );

protected:

  DICOMImageReader( void );
  ~DICOMImageReader( void );

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_FileNames;
  itk::GDCMImageIO::Pointer             m_ImageIO;

  /* Internal itkImageSeriesReader */
  typename ImageSeriesReaderType::Pointer        m_ImageSeriesReader;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  DirectoryNameType            m_ImageDirectoryName;
  DirectoryNameType            m_ImageDirectoryNameToBeSet;

  /** List of States */
  StateType                    m_IdleState;
  StateType                    m_ImageDirectoryNameReadState;
  StateType                    m_ImageReadState;


  /** List of State Inputs */
  InputType                    m_ReadImageRequestInput;
  InputType                    m_ImageDirectoryNameValidInput; 

  /** Error related state inputs */
  InputType                    m_ImageReadingErrorInput;
  InputType                    m_ImageDirectoryNameIsEmptyInput;
  InputType                    m_ImageDirectoryNameDoesNotExistInput;
  InputType                    m_ImageDirectoryNameIsNotDirectoryInput;
  InputType                    m_ImageDirectoryNameDoesNotHaveEnoughFilesInput;
 
  /** DICOM tags request inputs */

  InputType                    m_GetModalityInfoInput;
  InputType                    m_GetPatientNameInfoInput;
  
  void SetDirectoryName();

  void ReadDirectoryFileNames();

  /** This method request image read **/
  void AttemptReadImage();

  /** This method MUST be private in order to prevent unsafe access to the ITK
   * image level */
  virtual const ImageType * GetITKImage() const;
  
 /** The "ReportInvalidRequest" method throws InvalidRequestErrorEvent
 when invalid requests are made */
  void ReportInvalidRequest();

  /** This function reports an when the image directory is empty */
  void ReportImageDirectoryEmptyError();
 
  /** This function reports an error when image directory is non-existing */
  void ReportImageDirectoryDoesNotExistError();

 /* This function reports an error if the image directory doesn't have enough
  files */
  void ReportImageDirectoryDoesNotHaveEnoughFilesError();
  
  /** This function reports an error while image reading */
  void ReportImageReadingError();

  /** This function reports an error when the image directory name
  provided is not a directory containing DICOM series */
  void ReportImageDirectoryIsNotDirectoryError();

  /** This function throws a string loaded event. The string is loaded
  with DICOM  modality */
  void GetModalityInfo();

  /** This function throws a string loaded event. The string is loaded
  with patient name */
  void GetPatientNameInfo();

  char tmp_string[2048];
  
  DICOMInfoType m_PatientID;
  DICOMInfoType m_PatientName;
  DICOMInfoType m_Modality;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkDICOMImageReader.txx"
#endif

#endif // __igstkDICOMImageReader_h
