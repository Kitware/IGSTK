


#include "Application.h"

#include "FL/Fl_File_Chooser.H"

#include "vtkImageShiftScale.h"

#include "ClickedPointEvent.h"

Application
::Application()
{
  m_AxialViewer.SetOrientation(    ISIS::ImageSliceViewer::Axial    );
  m_CoronalViewer.SetOrientation(  ISIS::ImageSliceViewer::Coronal  );
  m_SaggitalViewer.SetOrientation( ISIS::ImageSliceViewer::Saggital );

  m_ShiftScaleImageFilter = vtkImageShiftScale::New();

  m_ShiftScaleImageFilter->SetInput(  m_ITK2VTKAdaptor->GetOutput() );
  m_ShiftScaleImageFilter->SetOutputScalarTypeToUnsignedChar();
  m_ShiftScaleImageFilter->ClampOverflowOn();

  m_AxialViewerCommand = itk::SimpleMemberCommand<Application>::New();
  m_AxialViewerCommand->SetCallbackFunction(this, &Application::ProcessAxialViewInteraction);
  m_AxialViewer.AddObserver(ISIS::ClickedPointEvent(), m_AxialViewerCommand);


  m_CoronalViewerCommand = itk::SimpleMemberCommand<Application>::New();
  m_CoronalViewerCommand->SetCallbackFunction(this, &Application::ProcessCoronalViewInteraction);
  m_CoronalViewer.AddObserver(ISIS::ClickedPointEvent(), m_CoronalViewerCommand);


  m_SaggitalViewerCommand = itk::SimpleMemberCommand<Application>::New();
  m_SaggitalViewerCommand->SetCallbackFunction(this, &Application::ProcessSaggitalViewInteraction);
  m_SaggitalViewer.AddObserver(ISIS::ClickedPointEvent(), m_SaggitalViewerCommand);
  
  m_DicomReaderCommand = itk::SimpleMemberCommand<Application>::New();
  m_DicomReaderCommand->SetCallbackFunction(this, &Application::ProcessDicomReaderInteraction);
  m_DicomVolumeReader.AddObserver( m_DicomReaderCommand );

}




Application
::~Application()
{
   if( m_ShiftScaleImageFilter )
     {
     m_ShiftScaleImageFilter->Delete();
     }
}



void
Application
::Show()
{
  mainWindow->show();
  axialView->show();
  coronalView->show();
  saggitalView->show();

  m_AxialViewer.SetInteractor( axialView );
  m_CoronalViewer.SetInteractor( coronalView );
  m_SaggitalViewer.SetInteractor( saggitalView );

  axialView->Initialize();
  coronalView->Initialize();
  saggitalView->Initialize();

}



void 
Application
::Hide()
{
  mainWindow->hide();
}



void 
Application
::Quit()
{
  this->Hide();
}




void 
Application
::Load()
{

  const char * filename = fl_file_chooser("Volume filename","*.*","");

  if( !filename  || strlen(filename) == 0 )
    {
    return;
    }
  
  m_VolumeReader->SetFileName( filename );

  m_VolumeReader->Update();

  m_LoadedVolume = m_VolumeReader->GetOutput();

  this->LoadPostProcessing();

}



void 
Application
::LoadDICOM()
{

  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");

  if( !directoryname  || strlen(directoryname) == 0 )
    {
    return;
    }
  
  m_DicomVolumeReader.SetDirectory( directoryname );

  // Attempt to read
  m_DicomVolumeReader.CollectSeriesAndSelectOne();

}



void 
Application
::LoadPostProcessing()
{

  m_RescaleIntensity->SetInput( m_LoadedVolume );

  try
    {
    m_ShiftScaleImageFilter->UpdateWholeExtent();
    }
  catch( itk::ExceptionObject & exp )
    {
    fl_message( exp.GetDescription() );
    return;
    }

  m_AxialViewer.SetInput(    m_ShiftScaleImageFilter->GetOutput() );
  m_CoronalViewer.SetInput(  m_ShiftScaleImageFilter->GetOutput() );
  m_SaggitalViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );

  const unsigned int numberOfZslices = m_LoadedVolume->GetBufferedRegion().GetSize()[2];
  const unsigned int numberOfYslices = m_LoadedVolume->GetBufferedRegion().GetSize()[1];
  const unsigned int numberOfXslices = m_LoadedVolume->GetBufferedRegion().GetSize()[0];

  axialViewSlider->bounds( 0.0, numberOfZslices-1 );
  axialViewSlider->value( numberOfZslices / 2 );
  this->SelectAxialSlice( numberOfZslices / 2 );

  coronalViewSlider->bounds( 0.0, numberOfYslices-1 );
  coronalViewSlider->value( numberOfYslices / 2 );
  this->SelectCoronalSlice( numberOfYslices / 2 );

  saggitalViewSlider->bounds( 0.0, numberOfXslices-1 );
  saggitalViewSlider->value( numberOfXslices / 2 );
  this->SelectSaggitalSlice( numberOfXslices / 2 );

  m_AxialViewer.Render();
  m_CoronalViewer.Render();
  m_SaggitalViewer.Render();
  
}



void 
Application
::SelectAxialSlice( int slice )
{
  m_AxialViewer.SelectSlice( slice );
  axialView->redraw();
  Fl::check();
}



void 
Application
::SelectCoronalSlice( int slice )
{
  m_CoronalViewer.SelectSlice( slice );
  coronalView->redraw();
  Fl::check();
}



void 
Application
::SelectSaggitalSlice( int slice )
{
  m_SaggitalViewer.SelectSlice( slice );
  saggitalView->redraw();
  Fl::check();
}



void  
Application
::ProcessDicomReaderInteraction( void )
{
 std::cout << "Processing Dicom Reader Interaction " << std::endl; 
  bool volumeIsLoaded = m_DicomVolumeReader.IsVolumeLoaded();

  if( volumeIsLoaded )
    {
    m_LoadedVolume = m_DicomVolumeReader.GetOutput();
    this->LoadPostProcessing();
    }
}



void 
Application
::ProcessAxialViewInteraction( void )
{
  m_AxialViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void  
Application
::ProcessCoronalViewInteraction( void )
{
  m_CoronalViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void  
Application
::ProcessSaggitalViewInteraction( void )
{
  m_SaggitalViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void 
Application
::SyncAllViews(void)
{
  itk::Point< double, 3 > point;
  point[0] = m_SeedPoint[0];
  point[1] = m_SeedPoint[1];
  point[2] = m_SeedPoint[2];

  
  VisualizationVolumeType::IndexType index; 

  m_LoadedVolume->TransformPhysicalPointToIndex( point, index );

  // If the point is outside the volume, do not change slice selection
  if( m_LoadedVolume->GetLargestPossibleRegion().IsInside( index ) )
    {
    axialViewSlider->value(    index[2]  );
    coronalViewSlider->value(  index[1]  );
    saggitalViewSlider->value( index[0]  );

    this->SelectAxialSlice(    index[2] );
    this->SelectCoronalSlice(  index[1] );
    this->SelectSaggitalSlice( index[0] );
    }

  // Sync the selected point in all the views even if it is outside the image
  m_AxialViewer.SelectPoint(    m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
  m_CoronalViewer.SelectPoint(  m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
  m_SaggitalViewer.SelectPoint( m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );

}

