
#include "igstkImageSpatialObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
ImageSpatialObject::ImageSpatialObject()
{
  // Create the image spatial object
  m_ImageSpatialObject = ImageSpatialObjectType::New();
  this->RequestSetSpatialObject( m_ImageSpatialObject );

  // Create a DICOM reader
  m_CTImageReader = CTImageReader::New();

} 

/** Destructor */
ImageSpatialObject::~ImageSpatialObject()  
{
  if( m_ImageSpatialObject )
    {
    m_ImageSpatialObject->Delete();
    }
}


const vtkImageData * ImageSpatialObject::GetVTKImageData() const
{
  if( m_CTImageReader )
    {
    return( m_CTImageReader->GetVTKImageData() );
    }
  else
    {
    return( NULL );
    }
}

void ImageSpatialObject::SetDirectory( const char *directory )
{
  if( m_CTImageReader )
    {
    m_CTImageReader->SetDirectory( directory );
    m_CTImageReader->RequestImageRead();
    }
}


/** Print Self function */
void ImageSpatialObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk

