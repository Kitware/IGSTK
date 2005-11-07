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

#include "igstkImageReader.h"

#include "igstkEvents.h"

#include "itkImageSeriesReader.h"
#include "itkEventObject.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"


namespace igstk
{

#define igstkUnsafeGetMacro(name,type) \
virtual const type & Get##name () const \
{ \
    igstkLogMacro( CRITICAL, "igstk::DICOMImageReader::Get" #name " unsafe method called...\n"); \
    return this->m_##name; \
}


itkEventMacro( DICOMModalityEvent,                                StringEvent);
itkEventMacro( DICOMPatientNameEvent,                             StringEvent);
itkEventMacro( DICOMImageReaderEvent,                             StringEvent);

// Invalid request error event 
itkEventMacro( DICOMInvalidRequestErrorEvent,                      DICOMImageReaderEvent );

// Events to handle errors with the ImageDirectory name
itkEventMacro(DICOMImageDirectoryEmptyErrorEvent,                  DICOMImageReaderEvent );
itkEventMacro(DICOMImageDirectoryDoesNotExistErrorEvent,           DICOMImageReaderEvent );
itkEventMacro(DICOMImageDirectoryIsNotDirectoryErrorEvent,         DICOMImageReaderEvent );
itkEventMacro(DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent, DICOMImageReaderEvent );

//Image reading error
itkEventMacro( DICOMImageReadingErrorEvent,                        DICOMImageReaderEvent );


  
/** \class DICOMImageReader
  * 
 * \brief This class reads DICOM files. 
 *
 * This class should not be instantiated directly, instead the derived classes
 * that are specific to particular image modalities should be used.
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


  /**  Run-time type information (and related methods). */
  igstkTypeMacro( DICOMImageReader, ImageReader );


  /** Type for representing the string of the directory 
   *  from which the DICOM files will be read. */
  typedef std::string    DirectoryNameType;
  
  /** Method to pass the directory name containing the DICOM image data */
  void RequestSetDirectory( const DirectoryNameType & directory );

  /** This method request image read **/
  void RequestReadImage();

  /** This function should be used to request modality info*/
  void RequestModalityInfo();

  /** This function will be used to request patient name info */
  void RequestPatientNameInfo();
  
  /** Logger class */
  typedef itk::Logger                  LoggerType;

  /** Declarations needed for the State Machine */
  igstkStateMachineTemplatedMacro();

  /** Type used for returning string values from the DICOM header */
  typedef std::string    DICOMInformationType;

  /** Precondition that should be invoked and verified before attempting to
   *  use the values of the methods GetPatientName(), GetPatientID() and
   *  GetModality().
   */
  bool FileSuccessfullyRead() const { return m_FileSuccessfullyRead; }
  
  /** Unsafe Get Macro for having access to the Patient Name.  This method is
   * considered unsafe because it is not subject to the control of the internal
   * state machine.  The method GetPatientName() should only be invoked if the
   * precondition method FileSuccessfullyRead() has already been called and it
   * has returned true. Calling GetPatientID() in any other situation will lead
   * to unpredictable behavior. */
  igstkUnsafeGetMacro( PatientName, DICOMInformationType );

  /** Unsafe Get Macro for having access to the Patient unique Identifier.
   * This method is considered unsafe because it is not subject to the control
   * of the internal state machine.  The method GetPatientID() should only be
   * invoked if the precondition method FileSuccessfullyRead() has already been
   * called and it has returned true. Calling GetPatientID() in any other
   * situation will lead to unpredictable behavior. */
  igstkUnsafeGetMacro( PatientID, DICOMInformationType );

  /** Unsafe Get Macro for having access to the image Modality.  This method is
   * considered unsafe because it is not subject to the control of the internal
   * state machine.  The method GetModality() should only be invoked if the
   * precondition method FileSuccessfullyRead() has already been called and it
   * has returned true. Calling GetModality() in any other situation will lead
   * to unpredictable behavior. */
  igstkUnsafeGetMacro( Modality, DICOMInformationType );
  
protected:

  DICOMImageReader( void );
  ~DICOMImageReader( void );

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_FileNames;
  itk::GDCMImageIO::Pointer             m_ImageIO;

  typedef typename Superclass::ImageType               ImageType;

  typedef itk::ImageSeriesReader< ImageType >          ImageSeriesReaderType;

  /* Internal itkImageSeriesReader */
  typename ImageSeriesReaderType::Pointer        m_ImageSeriesReader;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  DICOMImageReader(const Self&);    //purposely not implemented
  void operator=(const Self&);      //purposely not implemented


  DirectoryNameType            m_ImageDirectoryName;
  DirectoryNameType            m_ImageDirectoryNameToBeSet;

  /** List of States */
  StateType                    m_IdleState;
  StateType                    m_ImageDirectoryNameReadState;
  StateType                    m_AttemptingToReadImageState;
  StateType                    m_ImageReadState;


  /** List of State Inputs */
  InputType                    m_ReadImageRequestInput;
  InputType                    m_ImageDirectoryNameValidInput; 
  InputType                    m_ImageReadingSuccessInput;

  /** Error related state inputs */
  InputType                    m_ImageReadingErrorInput;
  InputType                    m_ImageDirectoryNameIsEmptyInput;
  InputType                    m_ImageDirectoryNameDoesNotExistInput;
  InputType                    m_ImageDirectoryNameIsNotDirectoryInput;
  InputType                    m_ImageDirectoryNameDoesNotHaveEnoughFilesInput;
 
  /** DICOM tags request inputs */

  InputType                    m_GetModalityInfoInput;
  InputType                    m_GetPatientNameInfoInput;
  
  /** Declarations needed for the Logger */
  igstkLoggerMacro();

  /** Set the name of the directory. To be invoked ONLY by the StateMachine */
  void SetDirectoryName();

  /** Invokes a FileNameGenerator in order to get the names of all the DICOM
      files in a directory. To be invoked ONLY by the StateMachine */
  void ReadDirectoryFileNames();

  /** This method request image read. To be invoked ONLY by the StateMachine. */
  void AttemptReadImage();

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

  /** This function reports success in image reading */
  void ReportImageReadingSuccess();

  /** This function reports an error when the image directory name
      provided is not a directory containing DICOM series */
  void ReportImageDirectoryIsNotDirectoryError();

  /** This function throws a string loaded event. The string is loaded
      with DICOM  modality */
  void GetModalityInfo();

  /** This function throws a string loaded event. The string is loaded
      with patient name */
  void GetPatientNameInfo();

  /** This method MUST be private in order to prevent 
      unsafe access to the ITK image level */
  virtual const ImageType * GetITKImage() const;


  /** Flag that indicates whether the file has been read successfully */
  bool                    m_FileSuccessfullyRead;

  /** Internal variables for holding patient and image specific information. */
  DICOMInformationType    m_PatientID;
  DICOMInformationType    m_PatientName;
  DICOMInformationType    m_Modality;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkDICOMImageReader.txx"
#endif

#endif // __igstkDICOMImageReader_h
