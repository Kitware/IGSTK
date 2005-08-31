#ifndef __igstkDICOMImageReader_txx
#define   __igstkDICOMImageReader_txx

#include "igstkDICOMImageReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
template <class TPixelType>
DICOMImageReader<TPixelType>::DICOMImageReader()
{
  // Create the DICOM GDCM file reader
  m_names = itk::GDCMSeriesFileNames::New();
  m_io = itk::GDCMImageIO::New();
  m_ImageSeriesReader = ImageSeriesReaderType::New();
} 

/** Destructor */
template <class TPixelType>
DICOMImageReader<TPixelType>::~DICOMImageReader()  
{
  /* 
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
*/
}

/** Read in the DICOM series image */
template <class TPixelType>
void DICOMImageReader<TPixelType>::SetDirectory( const char *directory )
{
  igstkLogMacro( DEBUG, "igstk::DICOMImageReader::SetDirectory called...\n");

  if( m_names && m_io && m_ImageSeriesReader )
    {
    igstkLogMacro( DEBUG, "Passing the input directory to the GDCM Series filename reader...\n");
    m_names->SetInputDirectory( directory );
    igstkLogMacro( DEBUG, "Passing input filenames to itk::ImageSeriesReader...\n");
    m_ImageSeriesReader->SetFileNames( m_names->GetInputFileNames() );
    igstkLogMacro( DEBUG, "Connecting the itk::ImageSeriesReader to GDCM ImageIO object...\n");
    m_ImageSeriesReader->SetImageIO( m_io );
    igstkLogMacro( DEBUG, "Reading the DICOM series...\n");
    m_ImageSeriesReader->Update();

    //this->m_itkExporter->SetInput( m_ImageSeriesReader->GetOutput() );
    }
}

/** Get the itk iamge data */
template <class TPixelType>
typename DICOMImageReader<TPixelType>::ImageConstPointer 
DICOMImageReader<TPixelType>::GetITKImageData() const
{
  return( m_ImageSeriesReader->GetOutput() );
}

/** Print Self function */
template <class TPixelType>
void DICOMImageReader<TPixelType>::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif
