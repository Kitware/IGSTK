
#include "ApplicationBase.h"

ApplicationBase::ApplicationBase()
{
  this->m_SeriesFileNames = SeriesFileNamesType::New();	

  this->m_ITK2VTKAdaptor = ITK2VTKAdaptorType::New();

  this->m_DicomReader.m_VolumeIsLoaded = false;
}

ApplicationBase::~ApplicationBase()
{

}

void ApplicationBase::LoadImageSeries( std::string seriesUID )
{
	this->m_DicomReader.ReadVolume( seriesUID );
//  this->m_ImageData = (ImageType::Pointer) ;
  
  this->m_ITK2VTKAdaptor->SetInput( this->m_DicomReader.GetOutput() );
}

std::string ApplicationBase::GetFirstSeriesUID( std::string dir )
{
  std::vector< std::string > series;

  this->m_SeriesFileNames->SetDirectory( dir );
  series = this->m_SeriesFileNames->GetSeriesUIDs();

  return series.front();
}

vtkImageData* ApplicationBase::GetVTKInput()
{
  if (!this->m_DicomReader.m_VolumeIsLoaded)
  {
    return NULL;
  }
  else
  {
    this->m_ITK2VTKAdaptor->Update();
    return this->m_ITK2VTKAdaptor->GetOutput();
  }
}
