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
  igstkLogMacro( CRITICAL, \
         "igstk::DICOMImageReader::Get" #name " unsafe method called...\n"); \
  return this->m_##name; \
}


itkEventMacro( DICOMModalityEvent,                                StringEvent);
itkEventMacro( DICOMPatientNameEvent,                             StringEvent);
itkEventMacro( DICOMImageReaderEvent,                             StringEvent);

// Invalid request error event 
itkEventMacro( DICOMInvalidRequestErrorEvent,
               DICOMImageReaderEvent );

// Events to handle errors with the ImageDirectory name
itkEventMacro(DICOMImageDirectoryEmptyErrorEvent,
              DICOMImageReaderEvent );
itkEventMacro(DICOMImageDirectoryDoesNotExistErrorEvent,
              DICOMImageReaderEvent );
itkEventMacro(DICOMImageDirectoryIsNotDirectoryErrorEvent,
              DICOMImageReaderEvent );
itkEventMacro(DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent,
              DICOMImageReaderEvent );

// Image series filename generation error event
itkEventMacro(DICOMImageSeriesFileNamesGeneratingErrorEvent,
              DICOMImageReaderEvent );

//Image reading error
itkEventMacro(DICOMImageReadingErrorEvent,
              DICOMImageReaderEvent );
  

/** \class DICOMImageReader
 * 
 * \brief This class reads DICOM files. 
 *
 * This class should not be instantiated directly, instead the derived 
 * classes that are specific to particular image modalities should be used.
 *
 * \image html  igstkDICOMImageReader.png  
 *           "DICOM Image Reader State Machine Diagram" 
 *
 * \image latex igstkDICOMImageReader.eps  
 *           "DICOM Image Reader State Machine Diagram" 
 * \ingroup Readers
 */
template <class TImageSpatialObject>
class DICOMImageReader : public ImageReader< TImageSpatialObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedAbstractClassTraitsMacro( DICOMImageReader, \
                                           ImageReader< TImageSpatialObject> )

public:

  /** Type for representing the string of the directory 
   *  from which the DICOM files will be read. */
  typedef std::string    DirectoryNameType;
  
  /** Method to pass the directory name containing the DICOM image data */
  void RequestSetDirectory( const DirectoryNameType & directory );

  /** This method request image read **/
  void RequestReadImage();

  /** This function should be used to request modality info*/
  void RequestGetModalityInformation();

  /** This function will be used to request patient name info */
  void RequestGetPatientNameInformation(); 

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

  /** Request to get the output image as an event */
  void RequestGetImage();
  
  /** Event type */
  igstkLoadedTemplatedObjectEventMacro( ImageModifiedEvent, IGSTKEvent, 
                                        TImageSpatialObject);

protected:

  DICOMImageReader( void );
  ~DICOMImageReader( void );

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_FileNames;
  itk::GDCMImageIO::Pointer             m_ImageIO;

  typedef typename Superclass::ImageType         ImageType;

  typedef itk::ImageSeriesReader< ImageType >    ImageSeriesReaderType;
  typedef itk::ImageFileReader< ImageType >      ImageReaderType;

  /** Internal itkImageSeriesReader */
  typename ImageSeriesReaderType::Pointer        m_ImageSeriesReader;
  typename ImageReaderType::Pointer              m_ImageFileReader;

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
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ImageDirectoryNameRead );
  igstkDeclareStateMacro( AttemptingToReadImage );
  igstkDeclareStateMacro( ImageSeriesFileNamesGenerated );
  igstkDeclareStateMacro( ImageRead );

  /** List of State Inputs */
  igstkDeclareInputMacro( ReadImage );
  igstkDeclareInputMacro( ImageDirectoryNameValid ); 
  igstkDeclareInputMacro( ImageReadingSuccess );
  igstkDeclareInputMacro( ImageSeriesFileNamesGeneratingSuccess );
  igstkDeclareInputMacro( ResetReader );
  igstkDeclareInputMacro( GetImage );
  
  /** Error related state inputs */
  igstkDeclareInputMacro( ImageReadingError );
  igstkDeclareInputMacro( ImageDirectoryNameIsEmpty );
  igstkDeclareInputMacro( ImageDirectoryNameDoesNotExist );
  igstkDeclareInputMacro( ImageDirectoryNameIsNotDirectory );
  igstkDeclareInputMacro( ImageDirectoryNameDoesNotHaveEnoughFiles );
  igstkDeclareInputMacro( ImageSeriesFileNamesGeneratingError );

  /** Check modality type 
   * This method is implemented in the derived classes to
   * check if the dicom data is read by the correct
   * Dicom image reader. For example, CT DICOM image 
   * should be read by CT image reader not by MRI image reader */
  virtual bool CheckModalityType( DICOMInformationType modality );

  
  /** DICOM tags request inputs */
  igstkDeclareInputMacro( GetModalityInformation );
  igstkDeclareInputMacro( GetPatientNameInformation );
  
  /** Declarations needed for the Logger */
  igstkLoggerMacro();

  /** Set the name of the directory. To be invoked ONLY by the StateMachine */
  void SetDirectoryNameProcessing();

  /** Invokes a FileNameGenerator in order to get the names of all the DICOM
   *  files in a directory. To be invoked ONLY by the StateMachine */
  void ReadDirectoryFileNamesProcessing();

  /** This method request image read. To be invoked ONLY by the StateMachine. */
  void AttemptReadImageProcessing();

  /** The "ReportInvalidRequest" method throws InvalidRequestErrorEvent
     when invalid requests are made */
  void ReportInvalidRequestProcessing();
  
  /** This function reports an when the image directory is empty */
  void ReportImageDirectoryEmptyErrorProcessing();
 
  /** This function reports an error when image directory is non-existing */
  void ReportImageDirectoryDoesNotExistErrorProcessing();

  /** This function reports an error if the image directory doesn't have enough
   *  files */
  void ReportImageDirectoryDoesNotHaveEnoughFilesErrorProcessing();
  
  /** This function reports an error in dicom image series file name 
   * generation */
  void ReportImageSeriesFileNamesGeneratingErrorProcessing();

  /** This function reports success in dicom image series file name 
   * generation */
  void ReportImageSeriesFileNamesGeneratingSuccessProcessing();

  /** This function reports an error while image reading */
  void ReportImageReadingErrorProcessing();

  /** This function reports success in image reading */
  void ReportImageReadingSuccessProcessing();

  /** This function reports the image */
  void ReportImageProcessing();

  /** This function resets the reader */
  void ResetReaderProcessing();

  /** This function reports an error when the image directory name
   *  provided is not a directory containing DICOM series */
  void ReportImageDirectoryIsNotDirectoryErrorProcessing();

  /** This function throws a string loaded event. The string is loaded
   *  with DICOM  modality */
  void GetModalityInformationProcessing();

  /** This function throws a string loaded event. The string is loaded
   *  with patient name */
  void GetPatientNameInformationProcessing();

  /** This method MUST be private in order to prevent 
   *  unsafe access to the ITK image level */
  virtual const ImageType * GetITKImage() const;

  /** Flag that indicates whether the file has been read successfully */
  bool                    m_FileSuccessfullyRead;

  /** Internal variables for holding patient and image specific information. */
  DICOMInformationType    m_PatientID;
  DICOMInformationType    m_PatientName;
  DICOMInformationType    m_Modality;
  DICOMInformationType    m_GantryTilt;

  /** Variable to hold image reading error information */
  std::string             m_ImageReadingErrorInformation;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkDICOMImageReader.txx"
#endif

#endif // __igstkDICOMImageReader_h
