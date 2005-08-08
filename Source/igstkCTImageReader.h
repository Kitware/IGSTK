
#ifndef __igstkCTImageReader_h
#define __igstkCTImageReader_h

#include "igstkMacros.h"
#include "igstkImageReader.h"
#include "itkObject.h"

namespace igstk
{

/** \class CTImageReader
 * 
 * \brief This class reads CT data stored in DICOM files. The parameters of the object
 * are the ... Default ...
 * 
 * \ingroup Object
 */

class CTImageReader : public ImageReader
{

public:

  /** Typedefs */
  typedef CTImageReader                         Self;
  typedef ImageReader                           Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;
  itkStaticConstMacro(ImageDimension, unsigned int, 3);
  typedef signed short                          PixelType;
  typedef itk::Image<PixelType, ImageDimension> ImageType;
  typedef itk::ImageFileReader< ImageType >     ImageFileReaderType;
  typedef itk::ImageSeriesReader< ImageType >   ImageSeriesReaderType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( CTImageReader, ImageReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** Method to set the directory containing the CT image data */
  void CTImageReader::SetDirectory( const char *directory );

  /** Method to get the ITK Image data */
  CTImageReader::ImageType::ConstPointer CTImageReader::GetITKImageData() const;

protected:

  CTImageReader( void );
  ~CTImageReader( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkImageFileReader */
  ImageFileReaderType::Pointer          m_ImageFileReader;

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_names;
  itk::GDCMImageIO::Pointer             m_io;

  //* Internal itkImageSeriesReader */
  ImageSeriesReaderType::Pointer        m_ImageSeriesReader;

};

} // end namespace igstk

#endif // __igstkCTImageReader_h
