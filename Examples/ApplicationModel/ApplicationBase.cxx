


#include "ApplicationBase.h"


ApplicationBase
::ApplicationBase()
{
  m_VolumeReader      = VolumeReaderType::New();
  m_RescaleIntensity  = RescaleIntensityFilterType::New();
  m_ITK2VTKAdaptor    = ITK2VTKAdaptorFilterType::New();
  m_ITK2VTKAdaptor->SetInput( m_RescaleIntensity->GetOutput() );

  m_RescaleIntensity->SetOutputMaximum( itk::NumericTraits< VisualizationPixelType >::max() );
  m_RescaleIntensity->SetOutputMinimum( itk::NumericTraits< VisualizationPixelType >::min() );

}




ApplicationBase
::~ApplicationBase()
{
}




void  
ApplicationBase::SetSeedPoint( double x, double y, double z )
{
  m_SeedPoint[0] = x;
  m_SeedPoint[1] = y;
  m_SeedPoint[2] = z;
}


void 
ApplicationBase::GetSeedPoint(double data[3])
{
  for(int i=0; i<3; i++)
  {
    data[i] = m_SeedPoint[i];
  }
}


