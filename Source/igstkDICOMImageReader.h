
#ifndef __igstkDICOMImageReader_h
#define __igstkDICOMImageReader_h

#include "igstkMacros.h"
#include "igstkImageReader.h"

#include "itkImageSeriesReader.h"
#include "itkObject.h"

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

  typedef itk::ImageSeriesReader<ImageType>     ImageSeriesReaderType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( DICOMImageReader, ImageReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** Method to set the directory containing the CT image data */
  void SetDirectory( const char *directory );

  /** Method to get the ITK Image data */
  ImageConstPointer GetITKImageData() const;

protected:

  DICOMImageReader( void );
  ~DICOMImageReader( void );

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_names;
  itk::GDCMImageIO::Pointer             m_io;

  /* Internal itkImageSeriesReader */
  typename ImageSeriesReaderType::Pointer        m_ImageSeriesReader;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkDICOMImageReader.txx"
#endif

#endif // __igstkDICOMImageReader_h
