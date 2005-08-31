#ifndef __igstkImageReader_txx
#define __igstkImageReader_txx

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
template <class TPixelType, unsigned int TDimension>
ImageReader<TPixelType,TDimension>::ImageReader()
{
 // Create the ITK image file reader
  m_ImageFileReader = ImageFileReaderType::New();
} 

/** Destructor */
template <class TPixelType, unsigned int TDimension>
ImageReader<TPixelType,TDimension>::~ImageReader()  
{

}

/** Print Self function */
template <class TPixelType, unsigned int TDimension>
void ImageReader<TPixelType,TDimension>::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

template <class TPixelType, unsigned int TDimension>
const vtkImageData * ImageReader<TPixelType,TDimension>::GetVTKImageData() const
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

} // end namespace igstk

#endif
