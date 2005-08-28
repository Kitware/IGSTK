
#ifndef __igstkImageReader_h
#define __igstkImageReader_h

#include "igstkMacros.h"
#include "itkObject.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"

#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"

#include "vtkImageData.h"

namespace igstk
{

/** \class ImageReader
 * 
 * \brief This class reads image data stored in files and provide pointers to the image data for use in either a VTK or an ITK pipeline. The parameters of the object
 * are the ... Default ...
 * 
 * \ingroup Object
 */

class ImageReader : public itk::Object
{

public:

  /** Typedefs */
  typedef ImageReader                           Self;
  typedef itk::Object                           Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;
  itkStaticConstMacro(ImageDimension, unsigned int, 3);
  typedef signed short                          PixelType;
  typedef itk::Image<PixelType, ImageDimension> ImageType;
  typedef itk::ImageFileReader< ImageType >     ImageFileReaderType;
  typedef itk::ImageSeriesReader< ImageType >   ImageSeriesReaderType;
  typedef itk::VTKImageExport< ImageType >      ExportFilterType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( ImageReader, itk::Object );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** method to get the ITK Image data */
//  virtual const ImageReader::ImageType::Pointer ImageReader::GetITKImageData() const

  /** Method to get the VTK image data */
  const vtkImageData * GetVTKImageData() const;


  /** Set method for the DICOM file name */
  void SetFileName( const char *filename );

  /** Returns true if the reader can open the file and the header is valid */
  bool CheckFileIsValid();

protected:

  ImageReader( void );
  ~ImageReader( void );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Classes to connect an ITK pipeline to a VTK pipeline */
  vtkImageImport                      * m_vtkImporter;
  ExportFilterType::Pointer             m_itkExporter;

};

} // end namespace igstk

#endif // __igstkImageReader_h
