/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDICOMGenericImageReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkDICOMGenericImageReader_h
#define __igstkDICOMGenericImageReader_h

#include "igstkImageReader.h"

#include "igstkEvents.h"

#include "itkImageSeriesReader.h"
#include "itkEventObject.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"


namespace igstk
{

igstkEventMacro( DICOMModalityEvent,             StringEvent);
igstkEventMacro( DICOMPatientNameEvent,          StringEvent);
igstkEventMacro( DICOMGenericImageReaderEvent,   StringEvent);

// Invalid request error event 
igstkEventMacro( DICOMInvalidRequestErrorEvent,
               DICOMGenericImageReaderEvent );

// Events to handle errors with the ImageDirectory name
igstkEventMacro(DICOMImageDirectoryEmptyErrorEvent,
              DICOMGenericImageReaderEvent );
igstkEventMacro(DICOMImageDirectoryDoesNotExistErrorEvent,
              DICOMGenericImageReaderEvent );
igstkEventMacro(DICOMImageDirectoryIsNotDirectoryErrorEvent,
              DICOMGenericImageReaderEvent );
igstkEventMacro(DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent,
              DICOMGenericImageReaderEvent );

// Image series filename generation error event
igstkEventMacro(DICOMImageSeriesFileNamesGeneratingErrorEvent,
              DICOMGenericImageReaderEvent );

//Image reading error
igstkEventMacro(DICOMImageReadingErrorEvent,
              DICOMGenericImageReaderEvent );
  

/** \class DICOMGenericImageReader
 * 
 * \brief This class reads DICOM files. 
 *
 * This class provides a DICOM reader that loads images of any modality.
 * This functionality is provided for prototyping applications and for
 * applications that are of very generic use.
 *
 * \warning We strongly discourage the use of this class for applications
 *          that are used in the surgery room because this class does not
 *          verifies the modality of the image data that is read.
 *
 *
 * \image html  igstkDICOMGenericImageReader.png  
 *           "DICOM Image Reader State Machine Diagram" 
 *
 * \image latex igstkDICOMGenericImageReader.eps  
 *           "DICOM Image Reader State Machine Diagram" 
 * \ingroup Readers
 */
class DICOMGenericImageReader : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( DICOMGenericImageReader, Object );

public:

  /** Type for representing the string of the directory 
   *  from which the DICOM files will be read. */
  typedef std::string    DirectoryNameType;
  
  /** Method to pass the directory name containing the DICOM image data */
  void RequestSetDirectory( const DirectoryNameType & directory );

  /** This method request image read */
  void RequestReadImage();

  /** This function should be used to request modality info */
  void RequestGetModalityInformation();

  /** This function will be used to request patient name info */
  void RequestGetPatientNameInformation(); 

  /** Type used for returning string values from the DICOM header */
  typedef std::string    DICOMInformationType;

  /** Request to get the output image as an event */
  void RequestGetImage();
  
  /** Event type */
  // igstkLoadedTemplatedObjectEventMacro( ImageModifiedEvent, IGSTKEvent,
  // TImageSpatialObject); 
  //
  // FIXME: Replace this with the EventMacro that sends
  // the typeless image.

protected:

  DICOMGenericImageReader( void );
  ~DICOMGenericImageReader( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  DICOMGenericImageReader(const Self&);    //purposely not implemented
  void operator=(const Self&);      //purposely not implemented

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_FileNames;
  itk::GDCMImageIO::Pointer             m_ImageIO;

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
  //
  // FIXME: replace with new image type
  //
  // virtual const ImageType * GetITKImage() const;


  /** Internal variables for holding patient and image specific information. */
  DICOMInformationType    m_PatientID;
  DICOMInformationType    m_PatientName;
  DICOMInformationType    m_Modality;
  DICOMInformationType    m_GantryTilt;

  /** Variable to hold image reading error information */
  std::string             m_ImageReadingErrorInformation;
};

} // end namespace igstk

#endif // __igstkDICOMGenericImageReader_h
