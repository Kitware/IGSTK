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
  
  /** Method to pass the directory name containing the DICOM image data */
  void RequestSetDirectory( const DirectoryNameType & directory );

  /** This method request image read **/
  void RequestReadImage();

  /* These Get functions should be converted to a get request inputs to
* the state machine */
  const char *             GetModality()     const ;
  const char *             GetPatientName()  const;

  /** Declarations needed for the State Machine */
  igstkStateMachineTemplatedMacro();

  /** Declarations needed for the Logger */
  igstkLoggerMacro();


protected:

  DICOMImageReader( void );
  ~DICOMImageReader( void );

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_FileNames;
  itk::GDCMImageIO::Pointer             m_ImageIO;

  /* Internal itkImageSeriesReader */
  typename ImageSeriesReaderType::Pointer        m_ImageSeriesReader;


  itkEventMacro( DICOMImageReaderEvent,                    IGSTKEvent);
  itkEventMacro( DICOMInvalidRequestErrorEvent,            DICOMImageReaderEvent );
  itkEventMacro( DICOMImageReadingErrorEvent,              DICOMImageReaderEvent );


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
  InputType                    m_ImageDirectoryNameIsEmptyInput;
  InputType                    m_ImageDirectoryNameDoesNotExistInput;
  InputType                    m_ImageDirectoryNameIsNotDirectoryInput;
  InputType                    m_ImageDirectoryNameHasNotEnoughFilesInput;

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


  char m_PatientName[2048 ];
  char m_PatientID[2048 ];
  char m_Modality[2048]; 

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkDICOMImageReader.txx"
#endif

#endif // __igstkDICOMImageReader_h
