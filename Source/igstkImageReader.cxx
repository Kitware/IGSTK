
#include "igstkImageReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/**
 * This function will connect the given itk::VTKImageExport filter to
 * the given vtkImageImport filter.
 */
template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

/** Constructor */
ImageReader::ImageReader()
{
  // Create classes for connecting ITK to VTK
  m_itkExporter = ExportFilterType::New();
  m_vtkImporter = vtkImageImport::New();
} 

/** Destructor */
ImageReader::~ImageReader()  
{
  if( m_itkExporter )
    {
    m_itkExporter = NULL;
    }

  if( m_vtkImporter )
    {
    m_vtkImporter->Delete();
    m_vtkImporter = NULL;
    }
}

//virtual const ImageReader::ImageType::Pointer ImageReader::GetITKImageData() const
//{

//}

const vtkImageData * ImageReader::GetVTKImageData() const
{
  if( m_vtkImporter && m_itkExporter )
    {
    // Connect vtkImageImport to the vtkImageExport instance
    ConnectPipelines( m_itkExporter, m_vtkImporter );

    m_itkExporter->Update();

    return( m_vtkImporter->GetOutput() );
    }
  else
    {
    return( NULL );
    }
}

/***
ImageReader::ImageSeriesReaderType::Pointer ImageReader::GetSeriesReader() const
{
  return( m_ImageSeriesReader );
}

void ImageReader::SetFileName( const char *filename )
{
  m_ImageFileReader->SetFileName( filename );

}

bool ImageReader::CheckFileIsValid()
{
  // Need appropriate call in ITK
  // m_ImageFileReader->?();

  return 1;
}

void ImageReader::Update()
{

  m_ImageFileReader->Update();

}
****/

/** Print Self function */
void ImageReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk

