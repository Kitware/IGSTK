
#ifndef __igstkImageSpatialObject_h
#define __igstkImageSpatialObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "igstkCTImageReader.h"

#include "itkImageSpatialObject.h"

namespace igstk
{

/** \class ImageSpatialObject
 * 
 * \brief This class represents an image object. The parameters of the object
 * are the ... Default ...
 * 
 * \ingroup Object
 */

class ImageSpatialObject 
: public SpatialObject
{

public:

  /** Typedefs */
  typedef ImageSpatialObject                                Self;
  typedef SpatialObject                                     Superclass;
  typedef itk::SmartPointer<Self>                           Pointer;
  typedef itk::SmartPointer<const Self>                     ConstPointer;
  itkStaticConstMacro(ImageDimension, unsigned int, 3);
  typedef signed short                                      PixelType;
  typedef itk::ImageSpatialObject< ImageDimension, PixelType > ImageSpatialObjectType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( ImageSpatialObject, SpatialObject );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** Set method for DICOM file name */
  void SetDirectory( const char *directory );

  /** Get the VTK image data (converted from the ITK pipeline) */
  const vtkImageData * GetVTKImageData() const;

protected:

  ImageSpatialObject( void );
  ~ImageSpatialObject( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal ImageReader */
  CTImageReader::Pointer                m_CTImageReader;

  /** Internal itkImageSpatialObject */
  ImageSpatialObjectType::Pointer       m_ImageSpatialObject;
};

} // end namespace igstk

#endif // __igstkImageSpatialObject_h
