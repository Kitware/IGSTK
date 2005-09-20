
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
 * \brief This class reads DICOM files. The parameters of the object
 * are the ... Default ...
 * 
 * \ingroup Object
 */

template <class TPixelType>
class DICOMImageReader : public ImageReader<TPixelType,3>
{

public:

  /** Typedefs */
  typedef DICOMImageReader                             Self;
  typedef ImageReader<TPixelType,3>                    Superclass;
  typedef itk::SmartPointer<Self>                      Pointer;
  typedef itk::SmartPointer<const Self>                ConstPointer;
  typedef typename Superclass::ImageType               ImageType;
  typedef typename ImageType::ConstPointer             ImageConstPointer;
  typedef itk::Logger                                  LoggerType;

  typedef itk::ImageSeriesReader<ImageType>     ImageSeriesReaderType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( DICOMImageReader, ImageReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** Method to pass the directory name containing the DICOM image data */
  void SetDirectory( const char *directory );

  /** This method request image read **/
  void RequestImageRead();

  /** This method requests image buffer emptied */
  void RequestEmptyImageBuffer();

  vtkImageData   *   GetVTKImageData() const;
  ImageConstPointer  GetITKImageData() const;

  /** Declarations needed for the State Machine */
  igstkStateMachineTemplatedMacro();

  /** Declarations needed for the Logger */
  igstkLoggerMacro();


protected:

  DICOMImageReader( void );
  ~DICOMImageReader( void );

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_names;
  itk::GDCMImageIO::Pointer             m_io;

  /* Internal itkImageSeriesReader */
  typename ImageSeriesReaderType::Pointer        m_ImageSeriesReader;


  itkEventMacro( DICOMImageReaderEvent,                    IGSTKEvent);
  itkEventMacro( InvalidRequestErrorEvent,                 DICOMImageReaderEvent );
  itkEventMacro( ImageReadingErrorEvent,                   DICOMImageReaderEvent );



  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  std::string                  m_ImageDirectoryName;
  /** List of States */
  StateType                    m_IdleState;
  StateType                    m_ImageDirectoryNameReadState;
  StateType                    m_ImageReadState;


  /** List of State Inputs */
  InputType                     m_ImageDirectoryNameInput;
  InputType                     m_ReadImageRequestInput;
  
  void ReadDirectoryName();

  /** This method request image read **/
  void ReadImage();

/** The "ReportInvalidRequest" method throws InvalidRequestErrorEvent
 when invalid requests are made */
  void ReportInvalidRequest();

  itkSetStringMacro(ImageDirectoryName);
  
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkDICOMImageReader.txx"
#endif

#endif // __igstkDICOMImageReader_h
