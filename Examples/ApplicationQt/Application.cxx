
#include "Application.h"

#include <vector>

#include "vtkQtRenderWindow.h"
#include "QSlider.h"

Application::Application()
{
  m_pAxialViewer = new ISIS::ImageSliceViewer( AxialViewer );
  m_pAxialViewer->SetInteractor( AxialViewerInteractor );
  m_pAxialViewer->SetOrientation( ISIS::ImageSliceViewer::Axial );
  AxialViewerInteractor->SetRenderWindow( AxialViewer );
  AxialViewerInteractor->Initialize();

  m_pCoronalViewer = new ISIS::ImageSliceViewer( CoronalViewer );
  m_pCoronalViewer->SetInteractor( CoronalViewerInteractor );
  m_pCoronalViewer->SetOrientation( ISIS::ImageSliceViewer::Coronal );
  CoronalViewerInteractor->SetRenderWindow( CoronalViewer );
  CoronalViewerInteractor->Initialize();

  m_pSagittalViewer = new ISIS::ImageSliceViewer( SagittalViewer );
  m_pSagittalViewer->SetInteractor( SagittalViewerInteractor );
  m_pSagittalViewer->SetOrientation( ISIS::ImageSliceViewer::Sagittal );
  SagittalViewerInteractor->SetRenderWindow( SagittalViewer );
  SagittalViewerInteractor->Initialize();

  m_pVolumeViewer = new IGSTK::IGMTVolumeViewer( VolumeViewer );
  m_pVolumeViewer->SetInteractor( VolumeViewerInteractor );
  VolumeViewerInteractor->SetRenderWindow( VolumeViewer );
  VolumeViewerInteractor->Initialize();  
}

Application::~Application()
{
  if (m_pAxialViewer)
  {
    delete m_pAxialViewer;
  }

  if (m_pCoronalViewer)
  {
    delete m_pCoronalViewer;
  }

  if (m_pSagittalViewer)
  {
    delete m_pSagittalViewer;
  }

  if (m_pVolumeViewer)
  {
    delete m_pVolumeViewer;
  }
}

void Application::OnLoad()
{
  std::string seriesUID;
	std::vector< std::string > filenames;
	char dir[1024];
  int ext[6];

	if (this->SelectFilesDirectory( dir ))
  {
    seriesUID = this->GetFirstSeriesUID( dir );
    if (seriesUID != "")
    {
      m_DicomReader.SetDirectory( dir );
	    this->LoadImageSeries( seriesUID ); 
      if (this->GetVTKInput())
      {
        this->GetVTKInput()->GetExtent(ext);
        this->SetAxialSliderRange(ext[4], ext[5]);
        this->SetAxialSliderPos((ext[4] + ext[5]) / 2);
        this->m_pAxialViewer->SetInput( this->GetVTKInput() );
        this->m_pAxialViewer->SelectSlice((ext[4] + ext[5]) / 2);
        this->m_pAxialViewer->Render();
        this->SetCoronalSliderRange(ext[2], ext[3]);
        this->SetCoronalSliderPos((ext[2] + ext[3]) / 2);
        this->m_pCoronalViewer->SetInput( this->GetVTKInput() );
        this->m_pCoronalViewer->SelectSlice((ext[2] + ext[3]) / 2);
        this->m_pCoronalViewer->Render();
        this->SetSagittalSliderRange(ext[0], ext[1]);
        this->SetSagittalSliderPos((ext[0] + ext[1]) / 2);
        this->m_pSagittalViewer->SetInput( this->GetVTKInput() );
        this->m_pSagittalViewer->SelectSlice((ext[0] + ext[1]) / 2);
        this->m_pSagittalViewer->Render();
      }
    }
  }
}

void Application::OnRender()
{
  if (this->GetVTKInput())
  {
    this->m_pVolumeViewer->SetInput( this->GetVTKInput() );
    this->m_pVolumeViewer->SetRenderingCategory( 4 );
    this->m_pVolumeViewer->Render();
  }
}

void Application::OnAxialSlider(int i)
{
  if (this->GetVTKInput())
  {
    m_pAxialViewer->SelectSlice(i);
    m_pAxialViewer->Render();
  }
}

void Application::OnCoronalSlider(int i)
{
  if (this->GetVTKInput())
  {
    m_pCoronalViewer->SelectSlice(i);
    m_pCoronalViewer->Render();
  }
}

void Application::OnSagittalSlider(int i)
{
  if (this->GetVTKInput())
  {
    m_pSagittalViewer->SelectSlice(i);
    m_pSagittalViewer->Render();
  }
}
