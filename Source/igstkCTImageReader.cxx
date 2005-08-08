
#include "igstkCTImageReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
CTImageReader::CTImageReader()
{
  // Create the ITK image file reader
  m_ImageFileReader = ImageFileReaderType::New();

  m_names = itk::GDCMSeriesFileNames::New();
  m_io = itk::GDCMImageIO::New();

  // Create the ITK image series reader for reading groups of images
  m_ImageSeriesReader = ImageSeriesReaderType::New();
} 

/** Destructor */
CTImageReader::~CTImageReader()  
{
  if( m_ImageFileReader )
    {
    m_ImageFileReader->Delete();
    }

  if( m_ImageSeriesReader )
    {
    m_ImageSeriesReader->Delete();
    }

  if( m_names )
    {
    m_names->Delete();
    }

  if( m_io )
    {
    m_io->Delete();
    }
}

void CTImageReader::SetDirectory( const char *directory )
{
  if( m_names && m_io && m_ImageSeriesReader )
    {
    m_names->SetInputDirectory( directory );
    m_ImageSeriesReader->SetFileNames( m_names->GetInputFileNames() );
    m_ImageSeriesReader->SetImageIO( m_io );
    m_ImageSeriesReader->Update();

    m_itkExporter->SetInput( m_ImageSeriesReader->GetOutput() );
    }
}

CTImageReader::ImageType::ConstPointer CTImageReader::GetITKImageData() const
{
  return( m_ImageSeriesReader->GetOutput() );
}

/** Print Self function */
void CTImageReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
