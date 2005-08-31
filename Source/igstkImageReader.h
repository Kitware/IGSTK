
#ifndef __igstkImageReader_h
#define __igstkImageReader_h

#include "igstkMacros.h"

#include "itkObject.h"
#include "itkMacro.h"
#include "itkImageFileReader.h"
#include "itkLogger.h"

#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkImageData.h"



namespace igstk
{

/** \class ImageReader
 * 
 * \brief This class reads image data stored in files and provide
 * pointers to the image data for use in either a VTK or an ITK
 * pipeline. This class has pixeltype and dimension template parameters
 *
 * \ingroup Object
 */

template <class TPixelType, unsigned int Dimension>
class ImageReader : public itk::Object
{

public:
  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;

  /** Typedefs */
  typedef ImageReader                           Self;
  typedef itk::Object                           Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int, Dimension);

  /** Some convenient typedefs for input image */
  typedef TPixelType                            PixelType;
  typedef itk::Image<PixelType,ImageDimension>  ImageType;
  typedef typename ImageType::ConstPointer      ImagePointer;
  typedef typename ImageType::RegionType        ImageRegionType; 
  typedef itk::ImageFileReader< ImageType >     ImageFileReaderType;
  typedef itk::VTKImageExport< ImageType >      ExportFilterType;

 /** Method to get the VTK image data */
  const vtkImageData * GetVTKImageData() const;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( ImageReader, itk::Object );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** method to get the ITK Image data */
//  virtual const ImageReader::ImageType::Pointer ImageReader::GetITKImageData() const

  /** Set method for the DICOM file name */
  void SetFileName( const char *filename );

  /** Returns true if the reader can open the file and the header is valid */
  bool CheckFileIsValid();

protected:

  ImageReader( void );
  ~ImageReader( void ) ;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /* Internal itkImageFileReader */
  typename ImageFileReaderType::Pointer      m_ImageFileReader;

  /** Classes to connect an ITK pipeline to a VTK pipeline */
  vtkImageImport                               * m_vtkImporter;
  typename ExportFilterType::Pointer             m_itkExporter;

  /** Declarations needed for the Logger */
  igstkLoggerMacro();

private:
  
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageReader.txx"
#endif

#endif // __igstkImageReader_h
