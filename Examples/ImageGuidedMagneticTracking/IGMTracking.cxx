
#include "IGMTracking.h"

#include "FL/Fl_File_Chooser.H"

#include "FL/Fl_Text_Display.h"

#include "vtkImageShiftScale.h"
#include "vtkPolyData.h"
#include "vtkVRMLExporter.h"

#include "ClickedPointEvent.h"

#include "Conversions.h"

#include <stdlib.h>
#include <conio.h>
#include "SYS\TIMEB.H"

bool	continueTracking;

IGMTracking::IGMTracking()
{
	m_AxialViewer.SetOrientation( ISIS::ImageSliceViewer::Axial    );
	m_CoronalViewer.SetOrientation( ISIS::ImageSliceViewer::Coronal    );
	m_SagittalViewer.SetOrientation( ISIS::ImageSliceViewer::Sagittal    );
	
	m_ShiftScaleImageFilter = vtkImageShiftScale::New();
  m_ShiftScaleImageFilter->SetInput(  m_ITK2VTKAdaptor->GetOutput() );
	m_ShiftScaleImageFilter->SetShift( 15 ); // 0
	m_ShiftScaleImageFilter->SetScale( 2.0 ); //1.5
	m_ShiftScaleImageFilter->SetOutputScalarTypeToUnsignedChar();
	m_ShiftScaleImageFilter->ClampOverflowOn();

  m_FusionShiftScaleImageFilter = vtkImageShiftScale::New();
  m_FusionShiftScaleImageFilter->SetInput(  m_FusionITK2VTKAdaptor->GetOutput() );
	m_FusionShiftScaleImageFilter->SetShift( 15 ); // 0
	m_FusionShiftScaleImageFilter->SetScale( 2.0 ); //1.5
	m_FusionShiftScaleImageFilter->SetOutputScalarTypeToUnsignedChar();
	m_FusionShiftScaleImageFilter->ClampOverflowOn();

  m_ImageBlend = vtkImageBlend::New();
  m_ImageBlend->SetInput(0, m_ShiftScaleImageFilter->GetOutput());
  m_ImageBlend->SetInput(1, m_FusionShiftScaleImageFilter->GetOutput());
  m_ImageBlend->SetOpacity(0, 1.0);
  m_ImageBlend->SetOpacity(1, 0.0);

  m_FusionOpacity = 0.5;

	m_AxialViewerCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_AxialViewerCommand->SetCallbackFunction(this, &IGMTracking::ProcessAxialViewInteraction);
	m_AxialViewer.AddObserver(ISIS::ClickedPointEvent(), m_AxialViewerCommand);

  m_AxialViewerMouseMoveCommand = vtkCallbackCommand::New();
	m_AxialViewerMouseMoveCommand->SetCallback(IGMTracking::ProcessAxialViewMouseMoveInteraction);
	m_AxialViewerMouseMoveCommand->SetClientData(this);
  AxialView->AddObserver(vtkCommand::MouseMoveEvent, m_AxialViewerMouseMoveCommand);

  m_AxialViewerRightClickCommand = vtkCallbackCommand::New();
	m_AxialViewerRightClickCommand->SetCallback(IGMTracking::ProcessAxialViewRightClickInteraction);
	m_AxialViewerRightClickCommand->SetClientData(this);
  AxialView->AddObserver(vtkCommand::RightButtonReleaseEvent, m_AxialViewerRightClickCommand);
	
	m_CoronalViewerCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_CoronalViewerCommand->SetCallbackFunction(this, &IGMTracking::ProcessCoronalViewInteraction);
	m_CoronalViewer.AddObserver(ISIS::ClickedPointEvent(), m_CoronalViewerCommand);

  m_CoronalViewerMouseMoveCommand = vtkCallbackCommand::New();
	m_CoronalViewerMouseMoveCommand->SetCallback(IGMTracking::ProcessCoronalViewMouseMoveInteraction);
	m_CoronalViewerMouseMoveCommand->SetClientData(this);
  CoronalView->AddObserver(vtkCommand::MouseMoveEvent, m_CoronalViewerMouseMoveCommand);

  m_CoronalViewerRightClickCommand = vtkCallbackCommand::New();
	m_CoronalViewerRightClickCommand->SetCallback(IGMTracking::ProcessCoronalViewRightClickInteraction);
	m_CoronalViewerRightClickCommand->SetClientData(this);
  CoronalView->AddObserver(vtkCommand::RightButtonReleaseEvent, m_CoronalViewerRightClickCommand);
	
	m_SagittalViewerCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_SagittalViewerCommand->SetCallbackFunction(this, &IGMTracking::ProcessSagittalViewInteraction);
	m_SagittalViewer.AddObserver(ISIS::ClickedPointEvent(), m_SagittalViewerCommand);

  m_SagittalViewerMouseMoveCommand = vtkCallbackCommand::New();
	m_SagittalViewerMouseMoveCommand->SetCallback(IGMTracking::ProcessSagittalViewMouseMoveInteraction);
	m_SagittalViewerMouseMoveCommand->SetClientData(this);
  SagittalView->AddObserver(vtkCommand::MouseMoveEvent, m_SagittalViewerMouseMoveCommand);

  m_SagittalViewerRightClickCommand = vtkCallbackCommand::New();
	m_SagittalViewerRightClickCommand->SetCallback(IGMTracking::ProcessSagittalViewRightClickInteraction);
	m_SagittalViewerRightClickCommand->SetClientData(this);
  SagittalView->AddObserver(vtkCommand::RightButtonReleaseEvent, m_SagittalViewerRightClickCommand);
	
	m_DicomReaderCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_DicomReaderCommand->SetCallbackFunction(this, &IGMTracking::ProcessDicomReaderInteraction);
	m_DicomVolumeReader.AddObserver( m_DicomReaderCommand );
  m_FusionDicomVolumeReader.AddObserver( m_DicomReaderCommand );

	m_ProgressCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ProgressCommand->SetCallbackFunction(this, IGMTracking::ProcessDicomReading);
	m_DicomVolumeReader.m_Reader->AddObserver(itk::ProgressEvent(), m_ProgressCommand);
  m_FusionDicomVolumeReader.m_Reader->AddObserver(itk::ProgressEvent(), m_ProgressCommand);

  m_ConnectedFilterProgressCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ConnectedFilterProgressCommand->SetCallbackFunction(this, IGMTracking::ProcessConnectedFilter);
  m_ConfidenceConnectedFilter->AddObserver(itk::ProgressEvent(), m_ConnectedFilterProgressCommand);
  m_ConnectedThresholdFilter->AddObserver(itk::ProgressEvent(), m_ConnectedFilterProgressCommand);
  m_NeighborhoodConnectedFilter->AddObserver(itk::ProgressEvent(), m_ConnectedFilterProgressCommand);
  m_IsolatedConnectedFilter->AddObserver(itk::ProgressEvent(), m_ConnectedFilterProgressCommand);

  m_ConnectedFilterStartCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ConnectedFilterStartCommand->SetCallbackFunction(this, IGMTracking::ProcessConnectedFilterStart);
  m_ConfidenceConnectedFilter->AddObserver(itk::StartEvent(), m_ConnectedFilterStartCommand);
  m_ConnectedThresholdFilter->AddObserver(itk::StartEvent(), m_ConnectedFilterStartCommand);
  m_NeighborhoodConnectedFilter->AddObserver(itk::StartEvent(), m_ConnectedFilterStartCommand);
  m_IsolatedConnectedFilter->AddObserver(itk::StartEvent(), m_ConnectedFilterStartCommand);

  m_ConnectedFilterEndCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ConnectedFilterEndCommand->SetCallbackFunction(this, IGMTracking::ProcessConnectedFilterEnd);
  m_ConfidenceConnectedFilter->AddObserver(itk::EndEvent(), m_ConnectedFilterEndCommand);
  m_ConnectedThresholdFilter->AddObserver(itk::EndEvent(), m_ConnectedFilterEndCommand);
  m_NeighborhoodConnectedFilter->AddObserver(itk::EndEvent(), m_ConnectedFilterEndCommand);
  m_IsolatedConnectedFilter->AddObserver(itk::EndEvent(), m_ConnectedFilterEndCommand);

  m_ResampleImagePreProgressCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ResampleImagePreProgressCommand->SetCallbackFunction(this, IGMTracking::ProcessResampleImagePreFilter);
  m_ResampleImagePreFilter->AddObserver(itk::ProgressEvent(), m_ResampleImagePreProgressCommand);

  m_ResampleImagePreStartCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ResampleImagePreStartCommand->SetCallbackFunction(this, IGMTracking::ProcessResampleImagePreFilterStart);
  m_ResampleImagePreFilter->AddObserver(itk::StartEvent(), m_ResampleImagePreStartCommand);
 
  m_ResampleImagePreEndCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ResampleImagePreEndCommand->SetCallbackFunction(this, IGMTracking::ProcessResampleImagePreFilterEnd);
  m_ResampleImagePreFilter->AddObserver(itk::EndEvent(), m_ResampleImagePreEndCommand);
 
  m_ResampleImagePostProgressCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ResampleImagePostProgressCommand->SetCallbackFunction(this, IGMTracking::ProcessResampleImagePostFilter);
  m_ResampleImagePostFilter->AddObserver(itk::ProgressEvent(), m_ResampleImagePostProgressCommand);

  m_ResampleImagePostStartCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ResampleImagePostStartCommand->SetCallbackFunction(this, IGMTracking::ProcessResampleImagePostFilterStart);
  m_ResampleImagePostFilter->AddObserver(itk::StartEvent(), m_ResampleImagePostStartCommand);
 
  m_ResampleImagePostEndCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ResampleImagePostEndCommand->SetCallbackFunction(this, IGMTracking::ProcessResampleImagePostFilterEnd);
  m_ResampleImagePostFilter->AddObserver(itk::EndEvent(), m_ResampleImagePostEndCommand);

	m_Overlay = false;
	m_ShowVolume = false; //false;//
	m_EntryPointSelected = false; 
	m_TargetPointSelected = false;
//	m_RefRecorded = false;
	m_ForceRecord = false;

  m_FullSizeIdx = -1;
  m_WindowType = 0;
  m_RenderingMethod = 0;
  m_RenderingCategory = 0;
  m_DesiredTriNum = 30000;
  m_Pipeline = 0;

  m_ProbeID = 0;

  m_Probe[0] = 1;
  m_Ref[0] = 0;
  m_UID[0] = 1042;
  m_Offset[0] = -12.6;
  m_Length[0] = 100.0;
  m_Radius[0] = 1.5;

  m_Probe[1] = 0;
  m_Ref[1] = 1;
  m_UID[1] = 1027;
  m_Offset[1] = -19.1;
  m_Length[1] = 100.0;
  m_Radius[1] = 1.5;

  m_Probe[2] = 0;
  m_Ref[2] = 0;
  m_UID[2] = 0;
  m_Offset[2] = 0.0;
  m_Length[2] = 10.0;
  m_Radius[2] = 1.0;

  m_Probe[3] = 0;
  m_Ref[3] = 0;
  m_UID[3] = 0;
  m_Offset[3] = 0.0;
  m_Length[3] = 10.0;
  m_Radius[3] = 1.0;

  int i;

  for (i = 0; i < PORTNUM; i++)
  {
    m_ProbeParameters.SetDeviceLine(i, m_Probe[i], m_Ref[i], m_UID[i], m_Offset[i], m_Length[i], m_Radius[i]);
  }

	this->SetNeedleLength( this->GetNeedleLength() );
	this->SetNeedleRadius( this->GetNeedleRadius() );

  m_VolumeType = 0;

  for (i = 0; i < 3; i++)
  {
    m_PointIndex[i] = 0;
  }  
}




IGMTracking::~IGMTracking()
{
	if( m_ShiftScaleImageFilter )
	{
		m_ShiftScaleImageFilter->Delete();
	}

  if( m_FusionShiftScaleImageFilter )
	{
		m_FusionShiftScaleImageFilter->Delete();
	}

  if (m_ImageBlend)
  {
    m_ImageBlend->Delete();
  }

  if (m_AxialViewerMouseMoveCommand)
  {
    m_AxialViewerMouseMoveCommand->Delete();
  }

  if (m_AxialViewerRightClickCommand)
  {
    m_AxialViewerRightClickCommand->Delete();
  }

  if (m_CoronalViewerMouseMoveCommand)
  {
    m_CoronalViewerMouseMoveCommand->Delete();
  }

  if (m_CoronalViewerRightClickCommand)
  {
    m_CoronalViewerRightClickCommand->Delete();
  }

  if (m_SagittalViewerMouseMoveCommand)
  {
    m_SagittalViewerMouseMoveCommand->Delete();
  }

  if (m_SagittalViewerRightClickCommand)
  {
    m_SagittalViewerRightClickCommand->Delete();
  }
}



void IGMTracking::Show()
{
	IGMTrackingGUI::Show();

	mainWindow->position((Fl::w() - mainWindow->w()) / 2, (Fl::h() - mainWindow->h()) * 2 / 3);

	AxialView->show();
	CoronalView->show();
	SagittalView->show();
	VolumeView->show();
  MotionView->show();
  MotionView->resize(0, 0, 0, 0);
  TargetView->show();
  TargetView->resize(0, 0, 0, 0);

	m_AxialViewer.SetInteractor( AxialView );
	m_CoronalViewer.SetInteractor( CoronalView );
	m_SagittalViewer.SetInteractor( SagittalView );
  m_VolumeViewer.SetInteractor( VolumeView );
	m_MotionViewer.SetInteractor( MotionView );
  m_TargetViewer.SetInteractor( TargetView );

  m_VolumeViewer.SetMCProgressMethod(MCProgressFunc, this);
 m_VolumeViewer.SetDecProgressMethod(DecProgressFunc, this);
  m_VolumeViewer.SetSmoothProgressMethod(SmoothProgressFunc, this);

	AxialView->Initialize();
	CoronalView->Initialize();
	SagittalView->Initialize();
	VolumeView->Initialize();
  MotionView->Initialize();
	m_MotionViewer.SetupCamera();
  TargetView->Initialize();

}



void IGMTracking::Hide()
{
	IGMTrackingGUI::Hide();
}



void IGMTracking::Quit()
{

  this->Hide();

	exit(0);

}




void IGMTracking::Load()
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



void IGMTracking::LoadDICOM()
{
	
	const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
	
	if( !directoryname  || strlen(directoryname) == 0 )
    {
		return;
    }
	
	sprintf(m_StringBuffer, "Directory chosen : %s", directoryname );
	this->PrintMessage(m_StringBuffer);

  m_VolumeType = 0;
	
	m_DicomVolumeReader.SetDirectory( directoryname );
	
	// Attempt to read
	m_DicomVolumeReader.CollectSeriesAndSelectOne();

	

}

void IGMTracking::FusionPostProcessing()
{
  unsigned int i;
  VolumeType::RegionType region;
  VolumeType::SizeType size, fussize;
  VolumeType::SpacingType spacing, resspacing;
  VolumeType::PointType origin;

  region = m_LoadedVolume->GetLargestPossibleRegion();
  size = region.GetSize();
  region = m_FusionVolume->GetLargestPossibleRegion();
  fussize = region.GetSize();
  spacing = m_FusionVolume->GetSpacing();
  origin = m_FusionVolume->GetOrigin();
  
  for (i = 0; i < 3; i++)
  {
    resspacing[i] = spacing[i] * fussize[i] / size[i];
  }  

  m_ResampleImageFusionFilter->SetInput( m_FusionVolume );
  m_ResampleImageFusionFilter->SetSize(size); 
  m_ResampleImageFusionFilter->SetOutputSpacing(resspacing);
  m_ResampleImageFusionFilter->SetOutputOrigin(origin);

  this->OnUpdateOpacity(0.5);

  this->SetInputData(2);

  this->RenderAllWindow();
}

void IGMTracking::LoadPostProcessing()
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

  this->SetInputData(0);

  float* spacing = m_ShiftScaleImageFilter->GetOutput()->GetSpacing();

  m_TargetViewer.SetSpacing(spacing[0], spacing[1], spacing[2]);

  VolumeViewSlider->bounds(0.0, m_ShiftScaleImageFilter->GetOutput()->GetScalarTypeMax());
  VolumeViewSlider->value(80);
	
	const unsigned int numberOfZslices = m_LoadedVolume->GetBufferedRegion().GetSize()[2];
	const unsigned int numberOfYslices = m_LoadedVolume->GetBufferedRegion().GetSize()[1];
	const unsigned int numberOfXslices = m_LoadedVolume->GetBufferedRegion().GetSize()[0];
	
	AxialViewSlider->bounds( 0.0, numberOfZslices-1 );
	AxialViewSlider->value( numberOfZslices / 2 );
	this->SelectAxialSlice( numberOfZslices / 2 );
	
	CoronalViewSlider->bounds( 0.0, numberOfYslices-1 );
	CoronalViewSlider->value( numberOfYslices / 2 );
	this->SelectCoronalSlice( numberOfYslices / 2 );
	
	SagittalViewSlider->bounds( 0.0, numberOfXslices-1 );
	SagittalViewSlider->value( numberOfXslices / 2 );
	this->SelectSagittalSlice( numberOfXslices / 2 );
	
	this->DisplaySelectedPosition( false );

	this->DisplayTargetPosition( false );

	this->DisplayEntryPosition( false );

	this->DisplayToolPosition( false );

	m_Overlay = false;
	m_EntryPointSelected = false; 
	m_TargetPointSelected = false;

	char name[1024];
	
	itk::DICOMImageIO2* pDICOMImageIO2 = (itk::DICOMImageIO2*)m_DicomVolumeReader.m_Reader->GetImageIO();
	
	pDICOMImageIO2->GetPatientName(name);
	
  m_AxialViewer.m_pAnnotation->SetPatientName(name);
  m_CoronalViewer.m_pAnnotation->SetPatientName(name);
  m_SagittalViewer.m_pAnnotation->SetPatientName(name);
  
  pDICOMImageIO2->GetPatientID(name);
	
  m_AxialViewer.m_pAnnotation->SetPatientID(name);
  m_CoronalViewer.m_pAnnotation->SetPatientID(name);
  m_SagittalViewer.m_pAnnotation->SetPatientID(name);

	pDICOMImageIO2->GetPatientSex(name);
	
  m_AxialViewer.m_pAnnotation->SetPatientSex(name);
  m_CoronalViewer.m_pAnnotation->SetPatientSex(name);
  m_SagittalViewer.m_pAnnotation->SetPatientSex(name);
	
  pDICOMImageIO2->GetPatientAge(name);
	
  m_AxialViewer.m_pAnnotation->SetPatientAge(name);
  m_CoronalViewer.m_pAnnotation->SetPatientAge(name);
  m_SagittalViewer.m_pAnnotation->SetPatientAge(name);
  
  pDICOMImageIO2->GetStudyDate(name);
	
  m_AxialViewer.m_pAnnotation->SetStudyDate(name);
  m_CoronalViewer.m_pAnnotation->SetStudyDate(name);
  m_SagittalViewer.m_pAnnotation->SetStudyDate(name);

	pDICOMImageIO2->GetModality(name);
	
  m_AxialViewer.m_pAnnotation->SetModality(name);
  m_CoronalViewer.m_pAnnotation->SetModality(name);
  m_SagittalViewer.m_pAnnotation->SetModality(name);
	
  pDICOMImageIO2->GetManufacturer(name);
	
  m_AxialViewer.m_pAnnotation->SetManufacturer(name);
  m_CoronalViewer.m_pAnnotation->SetManufacturer(name);
  m_SagittalViewer.m_pAnnotation->SetManufacturer(name);
	
  pDICOMImageIO2->GetInstitution(name);
	
  m_AxialViewer.m_pAnnotation->SetInstitution(name);
  m_CoronalViewer.m_pAnnotation->SetInstitution(name);
  m_SagittalViewer.m_pAnnotation->SetInstitution(name);
	
  pDICOMImageIO2->GetModel(name);
	
  m_AxialViewer.m_pAnnotation->SetModel(name);
  m_CoronalViewer.m_pAnnotation->SetModel(name);
  m_SagittalViewer.m_pAnnotation->SetModel(name);

  m_AxialViewer.Render();
  m_CoronalViewer.Render();
  m_SagittalViewer.Render();
  m_VolumeViewer.Render();

  this->AppendInfo("DICOM data loaded!");

}

void IGMTracking::SelectAxialSlice( int slice )
{
  m_AxialViewer.SelectSlice( slice );
  AxialView->redraw();
  Fl::check();
}



void IGMTracking::SelectCoronalSlice( int slice )
{
  m_CoronalViewer.SelectSlice( slice );
  CoronalView->redraw();
  Fl::check();
}



void IGMTracking::SelectSagittalSlice( int slice )
{
  m_SagittalViewer.SelectSlice( slice );
  SagittalView->redraw();
  Fl::check();
}


void IGMTracking::ProcessDicomReaderInteraction( void )
{
	this->PrintMessage( "Processing Dicom Reader Interaction " );
	bool volumeIsLoaded = m_DicomVolumeReader.IsVolumeLoaded();
	
	if( volumeIsLoaded )
    {
    if (m_VolumeType == 0)
    {
		  m_LoadedVolume = m_DicomVolumeReader.GetOutput();
		  this->LoadPostProcessing();
    }
    else
    {
      m_FusionVolume = m_FusionDicomVolumeReader.GetOutput();
      this->FusionPostProcessing();
    }
    }
}


void IGMTracking::DisplayEntryPosition( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateEntryPosition();
		m_CoronalViewer.ActivateEntryPosition();
		m_SagittalViewer.ActivateEntryPosition();
	}
	else
	{
		m_AxialViewer.DeActivateEntryPosition();
		m_CoronalViewer.DeActivateEntryPosition();
		m_SagittalViewer.DeActivateEntryPosition();
	}
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SagittalViewer.Render();
}
	


void IGMTracking::DisplaySelectedPosition( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateSelectedPosition();
		m_CoronalViewer.ActivateSelectedPosition();
		m_SagittalViewer.ActivateSelectedPosition();
	}
	else
	{
		m_AxialViewer.DeActivateSelectedPosition();
		m_CoronalViewer.DeActivateSelectedPosition();
		m_SagittalViewer.DeActivateSelectedPosition();
	}
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SagittalViewer.Render();
}


	
void IGMTracking::DisplayTargetPosition( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateTargetPosition();
		m_CoronalViewer.ActivateTargetPosition();
		m_SagittalViewer.ActivateTargetPosition();
	}
	else
	{
		m_AxialViewer.DeActivateTargetPosition();
		m_CoronalViewer.DeActivateTargetPosition();
		m_SagittalViewer.DeActivateTargetPosition();
	}

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SagittalViewer.Render();
}
/*
void IGMTracking::DisplayTipPosition( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateTipPosition();
		m_CoronalViewer.ActivateTipPosition();
		m_SagittalViewer.ActivateTipPosition();
	}
	else
	{
		m_AxialViewer.DeActivateTipPosition();
		m_CoronalViewer.DeActivateTipPosition();
		m_SagittalViewer.DeActivateTipPosition();
	}

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SagittalViewer.Render();
}
*/
void IGMTracking::DisplayEntryToTargetPath( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateEntryToTargetPathMarker();
		m_CoronalViewer.ActivateEntryToTargetPathMarker();
		m_SagittalViewer.ActivateEntryToTargetPathMarker();
	}
	else
	{
		m_AxialViewer.DeActivateEntryToTargetPathMarker();
		m_CoronalViewer.DeActivateEntryToTargetPathMarker();
		m_SagittalViewer.DeActivateEntryToTargetPathMarker();
	}

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SagittalViewer.Render();
}


void IGMTracking::DisplayToolPosition( const bool show)
{
	if (show)
	{
//		m_AxialViewer.ActivateToolPosition();
//		m_CoronalViewer.ActivateToolPosition();
//		m_SagittalViewer.ActivateToolPosition();

		m_AxialViewer.ActivateProbe(m_ProbeID);
		m_CoronalViewer.ActivateProbe(m_ProbeID);
		m_SagittalViewer.ActivateProbe(m_ProbeID);
	}
	else
	{
//		m_AxialViewer.DeActivateToolPosition();
//		m_CoronalViewer.DeActivateToolPosition();
//		m_SagittalViewer.DeActivateToolPosition();
    m_AxialViewer.DeActivateProbe(m_ProbeID);
		m_CoronalViewer.DeActivateProbe(m_ProbeID);
		m_SagittalViewer.DeActivateProbe(m_ProbeID);
	}

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SagittalViewer.Render();
}
	

void IGMTracking::ProcessAxialViewInteraction( void )
{
	m_AxialViewer.GetSelectPoint( m_ClickedPoint );
  m_AxialViewer.GetRightClickedPoint( m_RightClickedPoint );
	this->SyncAllViews( m_ClickedPoint );
	m_AxialViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_CoronalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_SagittalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );	
	this->DisplaySelectedPosition( true );

  char mes[128];
  sprintf(mes, "Image: %.2f %.2f %.2f", 
    m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2]);
  this->AppendInfo(mes);
}

void IGMTracking::ProcessAxialViewMouseMoveInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  float pos[3];
  IGMTracking* pTracking = (IGMTracking*)clientdata;

  if (pTracking->m_AxialViewer.m_Actor->GetInput() != NULL)
  {
    pTracking->m_AxialViewer.GetImagePosition(Fl::event_x(), Fl::event_y(), pos);
    pTracking->TrackingPoint(pos);
  } 
}

void IGMTracking::ProcessAxialViewRightClickInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  unsigned int i;
  float pos[3];
  IGMTracking* pTracking = (IGMTracking*)clientdata;

  if (pTracking->m_AxialViewer.m_Actor->GetInput() != NULL)
  {
   pTracking->m_AxialViewer.GetImagePosition(Fl::event_x(), Fl::event_y(), pos);
    for (i = 0; i < 3; i++)
    {
      pTracking->m_RightClickedPoint[i] = pos[i];
    }
    pTracking->m_AxialViewer.SelectPoint(pos[0], pos[1], pos[2]);
    pTracking->m_AxialViewer.Render();
  } 
}

void IGMTracking::ProcessCoronalViewMouseMoveInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  float pos[3];
  IGMTracking* pTracking = (IGMTracking*)clientdata;

  if (pTracking->m_CoronalViewer.m_Actor->GetInput() != NULL)
  {
    pTracking->m_CoronalViewer.GetImagePosition(Fl::event_x(), Fl::event_y(), pos);
    pTracking->TrackingPoint(pos);
  } 
}

void IGMTracking::ProcessCoronalViewRightClickInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  unsigned int i;
  float pos[3];
  IGMTracking* pTracking = (IGMTracking*)clientdata;

  if (pTracking->m_CoronalViewer.m_Actor->GetInput() != NULL)
  {
    pTracking->m_CoronalViewer.GetImagePosition(Fl::event_x(), Fl::event_y(), pos);
    for (i = 0; i < 3; i++)
    {
      pTracking->m_RightClickedPoint[i] = pos[i];
    }
    pTracking->m_CoronalViewer.SelectPoint(pos[0], pos[1], pos[2]);
    pTracking->m_CoronalViewer.Render();
  } 
}

void IGMTracking::ProcessSagittalViewMouseMoveInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  float pos[3];
  IGMTracking* pTracking = (IGMTracking*)clientdata;

  if (pTracking->m_SagittalViewer.m_Actor->GetInput() != NULL)
  {
    pTracking->m_SagittalViewer.GetImagePosition(Fl::event_x(), Fl::event_y(), pos);
    pTracking->TrackingPoint(pos);
  } 
}

void IGMTracking::ProcessSagittalViewRightClickInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  unsigned int i;
  float pos[3];
  IGMTracking* pTracking = (IGMTracking*)clientdata;

  if (pTracking->m_SagittalViewer.m_Actor->GetInput() != NULL)
  {
    pTracking->m_SagittalViewer.GetImagePosition(Fl::event_x(), Fl::event_y(), pos);
    for (i = 0; i < 3; i++)
    {
      pTracking->m_RightClickedPoint[i] = pos[i];
    }
    pTracking->m_SagittalViewer.SelectPoint(pos[0], pos[1], pos[2]);
    pTracking->m_SagittalViewer.Render();
  } 
}

void IGMTracking::ProcessCoronalViewInteraction( void )
{
	m_CoronalViewer.GetSelectPoint( m_ClickedPoint );
  m_CoronalViewer.GetRightClickedPoint( m_RightClickedPoint );
	this->SyncAllViews( m_ClickedPoint );
	m_AxialViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_CoronalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_SagittalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );	
	this->DisplaySelectedPosition( true );
}


void IGMTracking::ProcessSagittalViewInteraction( void )
{
	m_SagittalViewer.GetSelectPoint( m_ClickedPoint );
  m_SagittalViewer.GetRightClickedPoint( m_RightClickedPoint );
	this->SyncAllViews( m_ClickedPoint );
	m_AxialViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_CoronalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_SagittalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );	
	this->DisplaySelectedPosition( true );
}


void IGMTracking::SyncAllViews( const double aboutPoint[3] )
{
	itk::Point< double, 3 > point;
	
	point[0] = aboutPoint[0];
	point[1] = aboutPoint[1];
	point[2] = aboutPoint[2];
	
	VisualizationVolumeType::IndexType index; 
	
	m_LoadedVolume->TransformPhysicalPointToIndex( point, index );
	
	if( m_LoadedVolume->GetLargestPossibleRegion().IsInside( index ) )
  {
		AxialViewSlider->value(    index[2]  );
		CoronalViewSlider->value(  index[1]  );
		SagittalViewSlider->value( index[0]  );
		
		this->SelectAxialSlice(    index[2] );
		this->SelectCoronalSlice(  index[1] );
		this->SelectSagittalSlice( index[0] );
  }

	else
	{
		this->PrintMessage("SyncAllViews::Clicked point is outside the volume");
	}
	
}

int IGMTracking::TrackingPoint( const float aboutPoint[3], bool show )
{
  unsigned int i;
	itk::Point< float, 3 > point;
  char mes[128];
	
	point[0] = aboutPoint[0];
	point[1] = aboutPoint[1];
	point[2] = aboutPoint[2];
	
	VolumeType::IndexType index; 
  VolumeType::PixelType pixel;
	
	m_LoadedVolume->TransformPhysicalPointToIndex( point, index );
  
  if( m_LoadedVolume->GetLargestPossibleRegion().IsInside( index ) )
  {
    if (show)
    {
      pixel = m_LoadedVolume->GetPixel( index );
      sprintf(mes, "(%d %d %d) (%d)", index[0], index[1], index[2], pixel);
      m_ImagePosPane->value(mes);
    }

    for (i = 0; i < 3; i++)
    {
      m_PointIndex[i] = index[i];
    }
  }

  return (int)pixel;
}

void IGMTracking::OnTargetPoint( void )
{
	IGMTrackingBase::SetTargetPoint( m_ClickedPoint );
	sprintf(m_StringBuffer, "Target Point = [%4.3f %4.3f %4.3f]", 
		m_TargetPoint[0],  m_TargetPoint[1],  m_TargetPoint[2] );
	this->PrintMessage(m_StringBuffer);

	m_AxialViewer.m_TargetPositionMarker.SetCenter( m_ClickedPoint );
	m_CoronalViewer.m_TargetPositionMarker.SetCenter( m_ClickedPoint );
	m_SagittalViewer.m_TargetPositionMarker.SetCenter( m_ClickedPoint );

	this->DisplaySelectedPosition( false );
	this->DisplayTargetPosition( true );

	m_TargetPointSelected = true;
  
	if (m_EntryPointSelected)
	{
    m_AxialViewer.MakeEntryToTargetPathVisible();
    m_CoronalViewer.MakeEntryToTargetPathVisible();
    m_SagittalViewer.MakeEntryToTargetPathVisible();
		this->DisplayEntryToTargetPath( true );

    m_TargetViewer.SetEntryTargetPoint(m_EntryPoint, m_TargetPoint);
    m_TargetViewer.SetupCamera();
    m_TargetViewer.Render();
	}
}


void IGMTracking::OnEntryPoint( void )
{
	IGMTrackingBase::SetEntryPoint( m_ClickedPoint );
	sprintf(m_StringBuffer, "Entry Point = [%4.3f %4.3f %4.3f]", 
		m_EntryPoint[0],  m_EntryPoint[1],  m_EntryPoint[2] );
	this->PrintMessage(m_StringBuffer);
	
	m_AxialViewer.m_EntryPositionMarker.SetCenter( m_ClickedPoint );
	m_CoronalViewer.m_EntryPositionMarker.SetCenter( m_ClickedPoint );
	m_SagittalViewer.m_EntryPositionMarker.SetCenter( m_ClickedPoint );

	this->DisplaySelectedPosition( false );
	this->DisplayEntryPosition( true );

	m_EntryPointSelected = true; 
	if (m_TargetPointSelected)
	{
    m_AxialViewer.MakeEntryToTargetPathVisible();
    m_CoronalViewer.MakeEntryToTargetPathVisible();
    m_SagittalViewer.MakeEntryToTargetPathVisible();
		this->DisplayEntryToTargetPath( true );
  
    m_TargetViewer.SetEntryTargetPoint(m_EntryPoint, m_TargetPoint);
    m_TargetViewer.SetupCamera();
    m_TargetViewer.Render();
	}
  
}


void IGMTracking::SetNumberOfFiducials( unsigned int number )
{
	SetNumberOfFiducials( number );
	sprintf(m_StringBuffer, "Number of Fiducials = %d", number );
	this->PrintMessage(m_StringBuffer);
}



void IGMTracking::SetImageFiducial( unsigned int fiducialNumber )
{
	double   point[3];
	point[0] = m_ClickedPoint[0];
	point[1] = m_ClickedPoint[1];
	point[2] = m_ClickedPoint[2];
	
	IGMTrackingBase::SetImageFiducial( fiducialNumber, point );
	sprintf(m_StringBuffer, "Image Fiducials %d set at ( %4.3f, %4.3f, %4.3f)", 
		fiducialNumber + 1, m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	this->PrintMessage(m_StringBuffer);
}


	
void IGMTracking::SetTrackerFiducial( unsigned int fiducialNumber )
{
	IGMTrackingBase::SetTrackerFiducial( fiducialNumber, m_NeedlePosition );
	sprintf(m_StringBuffer, "Tracker Fiducials %d set at ( %4.3f, %4.3f, %4.3f)", 
		fiducialNumber + 1, m_NeedlePosition[0], m_NeedlePosition[1], m_NeedlePosition[2] );
	this->PrintMessage(m_StringBuffer);
}

	
void IGMTracking::InitializeTracker( const char *fileName )
{
	if (!m_AuroraTracker.InitializeTracker( fileName ))
	{
		this->PrintMessage("Initialization of Aurora Failed...");
//		fl_alert("Initialization of Aurora Failed...");
		this->AppendInfo("Initialization of Aurora Failed...");
	}
	else
	{
		this->PrintMessage("Initialization Aurora Succeeded...");
//		fl_alert("Initialization of Aurora Succeeded...");
		this->AppendInfo("Initialization of Aurora Succeeded...");
	}
}


void IGMTracking::ActivateTools( void )
{
	if (!m_AuroraTracker.ActivateTools())
	{
		this->PrintMessage("Activation of Aurora Tools Failed...");
//		fl_alert("Activation of Aurora Tools Failed...");
    this->AppendInfo("Activation of Aurora Tools Failed...");
	}
	else
	{
		this->PrintMessage("Activation of Aurora Tools Succeeded...");
//		fl_alert("Activation of Aurora Tools Succeeded...");
    this->AppendInfo("Activation of Aurora Tools Succeeded...");
	}
}


void IGMTracking::StartTracking( void )
{	
  int i;
  char toolID[10];
  char mes[128];

	m_AuroraTracker.StartTracking();
	
  for (i = 0; i < 4; i++)
  {
    if (m_Probe[i] == 1)
    {
      sprintf(toolID, "%02d", i + 1);
	    m_ToolHandle = m_AuroraTracker.GetMyToolHandle( toolID );
      printf("Probe:%d handle:%d\n", i, m_ToolHandle);
      if (m_ToolHandle<0)
      {
        sprintf(mes, "No tool on port %d detected ...", i);
//        fl_alert(mes);
        this->AppendInfo(mes);
      }
      else
      {
        sprintf(mes, "Tool on port %d recognised ...", i);
//        fl_alert(mes);
        this->AppendInfo(mes);
      }
    }      
  }

  sprintf(toolID, "%02d", m_ProbeID + 1);
	m_ToolHandle = m_AuroraTracker.GetMyToolHandle( toolID );

  if (m_UseReferenceNeedle)
	{
    sprintf(toolID, "%02d", m_RefID + 1 );
		m_ReferenceToolHandle = m_AuroraTracker.GetMyToolHandle( toolID );

    printf("Ref:%d handle:%d\n", m_RefID, m_ReferenceToolHandle);

		if (m_ReferenceToolHandle<0)
		{
      sprintf(mes, "No reference tool on port %d detected ...", m_RefID);
//			fl_alert(mes);
			this->AppendInfo(mes);
		}
		else
		{
      sprintf(mes, "Rreference tool on port %d recognised ...", m_RefID);
//			fl_alert(mes);
      this->AppendInfo(mes);
		}
	}
}

void IGMTracking::StopTracking( void )
{
	m_AuroraTracker.StopTracking();
  continueTracking = false;
  this->DisplayToolPosition(false);
}

void IGMTracking::PrintToolPosition( const int toolHandle )
{
	if ( m_AuroraTracker.m_HandleInformation[toolHandle].Xfrms.ulFlags == TRANSFORM_VALID )
	{
		if ( m_AuroraTracker.m_HandleInformation[toolHandle].HandleInfo.bPartiallyOutOfVolume )
			this->PrintMessage("POOV" );
		else if ( m_AuroraTracker.m_HandleInformation[toolHandle].HandleInfo.bOutOfVolume )
			this->PrintMessage("OOV");
		else
			this->PrintMessage("OK");
	}
	else 
	{
		if ( m_AuroraTracker.m_HandleInformation[toolHandle].Xfrms.ulFlags == TRANSFORM_MISSING )
			this->PrintMessage("MISSING" );
		else
			this->PrintMessage("DISABLED" );	
	}				
}

	
void IGMTracking::OnToolPosition( void )
{
	m_AuroraTracker.UpdateToolStatus();

	m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ToolHandle, m_NeedleTipOffset, m_NeedlePosition);

	this->PrintToolPosition( m_ToolHandle ); 
}


void CheckKeyPress(void *dummy)
{
	_getch();
//	printf("Key Press detected ****************\n");
	continueTracking = false;
//	_endthread();
}


bool IGMTracking::OnRegister( void )
{
	bool ret = this->RegisterImageWithTrackerAndComputeRMSError();

  this->AppendInfo(m_StringBuffer);
  
  return ret;
}


void IGMTracking::OnTrackToolPosition( void )
{
	static   int run = 0;

	if (_beginthread(CheckKeyPress, 0, NULL) != -1)
	{
		this->PrintMessage("Thread Successfully Created. Press any key to exit thread...");
		printf("Thread started. Press any key to exit thread ...\n");

		FILE * fp;
		fp = fopen("track_position.dat", "a+");
		
		sprintf(m_StringBuffer, "***********   RUN %.3d DATA   *********\n", ++run );	
		if (fp != NULL) fprintf(fp, m_StringBuffer);
		printf( m_StringBuffer );
		
		continueTracking = true;

		while(continueTracking)
		{
			m_AuroraTracker.UpdateToolStatus();
			m_AuroraTracker.GetToolPosition( m_ToolHandle, m_NeedlePosition);
			sprintf(m_StringBuffer, "%4.3f   %4.3f   %4.3f\n", m_NeedlePosition[0], m_NeedlePosition[1], m_NeedlePosition[2]);
			if (fp != NULL) fprintf(fp, m_StringBuffer);
			printf( m_StringBuffer );
			Sleep(10);
		}

		if (fp!=NULL) 
		{
			fprintf(fp, "\n\n\n\n\n");
			fclose(fp);
		}

		this->PrintMessage("Thread Stopped....");
		printf("Thread Stopped...\n");
	}
	else
	{
		this->PrintMessage("Thread Creation Failed.");
	}
}


void IGMTracking::OnOverlay( void )
{
	static   int run = 0;
  unsigned int i;
	bool firstpos = true;
//<<<<<<< IGMTracking.cxx
	double HomePos[3], d[3], dis, motiondis;
	double toolPositionInImageSpace[3], anotherPositionInImageSpace[3];
  float spacing[3];
  double anotherPosition[3];
  float NoRefPosition1[3], NoRefPosition2[3];
  double refToolNewPosition[3], offset[3], offsetInImageSpace1[3], offsetInImageSpace2[3];
  double tippos[3], hippos[3];
  char mes[128];
//=======
//	double HomePos[3], d[3], dis;
//	double toolPositionInImageSpace[3], anotherPositionInImageSpace[3];
// >>>>>>> 1.6

	
	if (_beginthread(CheckKeyPress, 0, NULL) != -1)
	{
		this->PrintMessage("Thread Successfully Created. Press any key to exit thread...");
		printf("Thread started. Press any key to exit thread ...\n");

		FILE * fp;
		fp = fopen("overlay_track_position.dat", "a+");
		
		sprintf(m_StringBuffer, "***********   OVERLAY RUN %.3d DATA   *********\n", ++run );	
		if (fp != NULL) fprintf(fp, m_StringBuffer);
		printf( m_StringBuffer );

		if (m_UseReferenceNeedle && !m_ForceRecord)
		{
			m_AuroraTracker.UpdateToolStatus();
			m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ReferenceToolHandle, 
				m_ReferenceNeedleTipOffset, m_ReferenceNeedlePosition);

			memcpy(HomePos, m_ReferenceNeedlePosition, 3 * sizeof(double));
		}

		m_Overlay = true;
		continueTracking = true;
		this->DisplayToolPosition( true );

		while(continueTracking)
		{
			this->OnToolPosition();
//<<<<<<< IGMTracking.cxx
		
//=======
			
//			double anotherPosition[3];
			
//>>>>>>> 1.6
			m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ToolHandle, m_NeedleEndOffset, anotherPosition);
			
			if (m_UseReferenceNeedle)
			{
//<<<<<<< IGMTracking.cxx
//=======
				//double refToolNewPosition[3], offset[3];
//>>>>>>> 1.6
				m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ReferenceToolHandle, 
					m_ReferenceNeedleTipOffset, refToolNewPosition);
				
				for (i=0; i<3; i++)
				{
					offset[i] = refToolNewPosition[i] - m_ReferenceNeedlePosition[i];
          NoRefPosition1[i] = m_NeedlePosition[i];
					m_NeedlePosition[i] -= offset[i];
          NoRefPosition2[i] = anotherPosition[i];
					anotherPosition[i] -= offset[i];
				}
		
        if (m_WindowType == 1)
        {
          for (i = 0; i < 3; i++)
          {
            d[i] = HomePos[i] - refToolNewPosition[i];
          }
          
          motiondis = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
          m_MotionViewer.UpdateMotion(motiondis);
        }
			}
			
//<<<<<<< IGMTracking.cxx
//=======
//			double toolPositionInImageSpace[3], anotherPositionInImageSpace[3];
			
//>>>>>>> 1.6
			m_FiducialRegistration.FromSourceSpaceToDestinationSpace(m_NeedlePosition,toolPositionInImageSpace);
			m_FiducialRegistration.FromSourceSpaceToDestinationSpace(anotherPosition,anotherPositionInImageSpace);
		      
      sprintf(mes, "(%.2f %.2f %.2f)", 
        toolPositionInImageSpace[0], toolPositionInImageSpace[1], toolPositionInImageSpace[2]);
      m_ProbePosPane->value(mes);

      m_ShiftScaleImageFilter->GetOutput()->GetSpacing(spacing);

      dis = 0;
      for (i = 0; i < 3; i++)
      {
        d[i] = toolPositionInImageSpace[i] - m_ClickedPoint[i];
        dis += d[i] * d[i];
      }
      sprintf(mes, "%.3fmm", sqrt(dis));
      m_DistancePane->value(mes);

      if (m_FullSizeIdx != 3)
      {
        m_AxialViewer.DeActivateSelectedPosition();
        m_CoronalViewer.DeActivateSelectedPosition();
        m_SagittalViewer.DeActivateSelectedPosition();

        m_AxialViewer.SetProbeTipAndDirection(m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
        m_AxialViewer.MakeToolPositionMarkerVisible();
        
        m_CoronalViewer.SetProbeTipAndDirection(m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
        m_CoronalViewer.MakeToolPositionMarkerVisible();
        
        m_SagittalViewer.SetProbeTipAndDirection(m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
        m_SagittalViewer.MakeToolPositionMarkerVisible();
        
        this->SyncAllViews( toolPositionInImageSpace );
      }

//      this->TrackingPoint(toolPositionInImageSpace, false);

      if (m_WindowType == 0 && m_VolumeViewer.GetProbeVisibility() == 1)
      {
        m_VolumeViewer.SetProbeTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
        m_VolumeViewer.Render();
      }
  
      if (m_WindowType == 2)
      {
        if (m_UseReferenceNeedle)
        {
          m_FiducialRegistration.FromSourceSpaceToDestinationSpace(NoRefPosition1, toolPositionInImageSpace);
          m_FiducialRegistration.FromSourceSpaceToDestinationSpace(NoRefPosition2, anotherPositionInImageSpace);
          m_FiducialRegistration.FromSourceSpaceToDestinationSpace(refToolNewPosition, offsetInImageSpace2);
          m_FiducialRegistration.FromSourceSpaceToDestinationSpace(m_ReferenceNeedlePosition, offsetInImageSpace1);

  //        printf("off: %.2f %.2f %.2f\n", offset[0], offset[1], offset[2]);
 //         printf("offimage: %.2f %.2f %.2f\n", offsetInImageSpace[0], offsetInImageSpace[1], offsetInImageSpace[2]);
          
          for (i = 0; i < 3; i++)
          {
            tippos[i] = toolPositionInImageSpace[i];
            hippos[i] = anotherPositionInImageSpace[i];
          }
          m_TargetViewer.SetProbePosition(tippos, hippos);
          m_TargetViewer.TranslateTargetPoint(offsetInImageSpace2[0] - offsetInImageSpace1[0], 
            offsetInImageSpace2[1] - offsetInImageSpace1[1], 
            offsetInImageSpace2[2] - offsetInImageSpace1[2]);
          m_TargetViewer.Render();
        }  
        else
        {
          for (i = 0; i < 3; i++)
          {
            tippos[i] = toolPositionInImageSpace[i];
            hippos[i] = anotherPositionInImageSpace[i];
          }
          m_TargetViewer.SetProbePosition(tippos, hippos);
          m_TargetViewer.Render();
        }      
      }     
      
			sprintf(m_StringBuffer, "%4.3f   %4.3f   %4.3f\n", toolPositionInImageSpace[0], toolPositionInImageSpace[1], toolPositionInImageSpace[2]);
			if (fp != NULL) fprintf(fp, m_StringBuffer);	  	
		
			Sleep(10);
		}

		if (fp!=NULL) 
		{
			fprintf(fp, "\n\n\n\n\n");
			fclose(fp);
		}

		this->PrintMessage("Thread Stopped....");
		printf("Thread Stopped...\n");
	}
	else
	{
		this->PrintMessage("Thread Creation Failed.");
	}
}

//<<<<<<< IGMTracking.cxx
//float IGMTracking::GetImageScale( void )
//=======
void IGMTracking::OnMotionTracking( void )
{
	static   int run = 0;
	bool firstpos = true;
	double HomePos[3], d[3], dis;
//	double toolPositionInImageSpace[3];
	double refPositionInImageSpace[3];

	if (_beginthread(CheckKeyPress, 0, NULL) != -1)
	{
		this->PrintMessage("Thread Successfully Created. Press any key to exit thread...");
		printf("Thread started. Press any key to exit thread ...\n");

		if (m_UseReferenceNeedle)
		{
			m_AuroraTracker.UpdateToolStatus();
			m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ReferenceToolHandle, 
				m_ReferenceNeedleTipOffset, m_ReferenceNeedlePosition);
		}

		m_Overlay = true;
		continueTracking = true;
//		this->DisplayToolPosition( true );

		while(continueTracking)
		{
			this->OnToolPosition();
			
			double anotherPosition[3];

	//		m_AuroraTracker.UpdateToolStatus();
			
			m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ToolHandle, m_NeedleEndOffset, anotherPosition);
			
			if (m_UseReferenceNeedle)
			{
				double refToolNewPosition[3];
				m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ReferenceToolHandle, 
					m_ReferenceNeedleTipOffset, refToolNewPosition);

				sprintf(m_StringBuffer, "Port0: %4.3f   %4.3f   %4.3f\nPort1: %4.3f   %4.3f   %4.3f\n", 
					anotherPosition[0], anotherPosition[1], anotherPosition[2], refToolNewPosition[0], refToolNewPosition[1], refToolNewPosition[2]);
				printf( m_StringBuffer );
				
//				for(int i=0; i<3; i++)
//				{
//					offset[i] = refToolNewPosition[i] - m_ReferenceNeedlePosition[i];
//					m_NeedlePosition[i] -= offset[i];
//					anotherPosition[i] -= offset[i];
//				}

				m_FiducialRegistration.FromSourceSpaceToDestinationSpace(refToolNewPosition,refPositionInImageSpace);
//				m_FiducialRegistration.FromSourceSpaceToDestinationSpace(anotherPosition,anotherPositionInImageSpace);

				if (firstpos)
				{
					memcpy(HomePos, refPositionInImageSpace, 3 * sizeof(double));
					firstpos = false;
				}
				
				for (int i = 0; i < 3; i++)
				{
					d[i] = HomePos[i] - refPositionInImageSpace[i];
				}
				
				dis = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
				m_MotionViewer.UpdateMotion(dis);

				sprintf(m_StringBuffer, "Dis: %4.3f\n", dis);
				printf( m_StringBuffer );
			}
			
//			m_FiducialRegistration.FromSourceSpaceToDestinationSpace(m_NeedlePosition,toolPositionInImageSpace);
//			m_FiducialRegistration.FromSourceSpaceToDestinationSpace(anotherPosition,anotherPositionInImageSpace);
			
//			m_AxialViewer.m_ToolPositionMarker.SetTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
//			m_CoronalViewer.m_ToolPositionMarker.SetTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
//			m_SaggitalViewer.m_ToolPositionMarker.SetTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
			
//			sprintf(m_StringBuffer, "%4.3f   %4.3f   %4.3f\n", toolPositionInImageSpace[0], toolPositionInImageSpace[1], toolPositionInImageSpace[2]);
//			printf( m_StringBuffer );

//			this->SyncAllViews( toolPositionInImageSpace );
			
			Sleep(10);
		}

		this->PrintMessage("Thread Stopped....");
		printf("Thread Stopped...\n");
	}
	else
	{
		this->PrintMessage("Thread Creation Failed.");
	}
}


double IGMTracking::GetImageScale( void )
//>>>>>>> 1.6
{
	return m_ShiftScaleImageFilter->GetScale();
}
	
void IGMTracking::SetImageScale( double val )
{
	m_ShiftScaleImageFilter->SetScale( val );
	m_ShiftScaleImageFilter->UpdateWholeExtent();

  m_FusionShiftScaleImageFilter->SetScale( val );
	m_FusionShiftScaleImageFilter->UpdateWholeExtent();

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SagittalViewer.Render();
}

	
double IGMTracking::GetImageShift( void )
{
	return m_ShiftScaleImageFilter->GetShift();
}
	
void IGMTracking::SetImageShift( double val )
{
	m_ShiftScaleImageFilter->SetShift( val );
	m_ShiftScaleImageFilter->UpdateWholeExtent();

  m_FusionShiftScaleImageFilter->SetShift( val );
	m_FusionShiftScaleImageFilter->UpdateWholeExtent();

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SagittalViewer.Render();
}

void IGMTracking::SetNeedleLength( const double val )
{
	IGMTrackingBase::SetNeedleLength( val );
//	m_AxialViewer.m_ToolPositionMarker.SetLength(val);
//	m_CoronalViewer.m_ToolPositionMarker.SetLength(val);
//	m_SagittalViewer.m_ToolPositionMarker.SetLength(val);
}
	
void IGMTracking::SetNeedleRadius( const double val )
{
	IGMTrackingBase::SetNeedleRadius( val );
//	m_AxialViewer.m_ToolPositionMarker.SetRadius(val);
//	m_CoronalViewer.m_ToolPositionMarker.SetRadius(val);
//	m_SagittalViewer.m_ToolPositionMarker.SetRadius(val);
}

void IGMTracking::SetProbeLength( int i, const float val )
{
//	IGMTrackingBase::SetNeedleLength( val );
  m_AxialViewer.m_ProbeMarker[i].SetLength(val);
  m_CoronalViewer.m_ProbeMarker[i].SetLength(val);
  m_SagittalViewer.m_ProbeMarker[i].SetLength(val);
}
	
void IGMTracking::SetProbeRadius( int i, const float val )
{
//	IGMTrackingBase::SetNeedleRadius( val );
	m_AxialViewer.m_ProbeMarker[i].SetRadius(val);
	m_CoronalViewer.m_ProbeMarker[i].SetRadius(val);
	m_SagittalViewer.m_ProbeMarker[i].SetRadius(val);
}	

void IGMTracking::PrintDebugInfo( void )
{
	double pos[3];
	printf("Debug Information:\n");
//<<<<<<< IGMTracking.cxx
	m_SagittalViewer.m_TargetPositionMarker.GetCenter( pos1 );
	printf("Target: %4.3f   %4.3f   %4.3f\n", pos1[0], pos1[1], pos1[2]);
	m_SagittalViewer.m_EntryPositionMarker.GetCenter( pos1 );
	printf("Entry: %4.3f   %4.3f   %4.3f\n", pos1[0], pos1[1], pos1[2]);
	m_SagittalViewer.m_EntryToTargetPathMarker.m_LineSource->GetPoint1( pos1 );
	m_SagittalViewer.m_EntryToTargetPathMarker.m_LineSource->GetPoint2( pos2 );
	printf("Line Point 01: %4.3f   %4.3f   %4.3f\n", pos1[0], pos1[1], pos1[2]);
	printf("Line Point 02: %4.3f   %4.3f   %4.3f\n", pos2[0], pos2[1], pos2[2]);
/*=======
	m_SaggitalViewer.m_TargetPositionMarker.GetCenter( pos );
	printf("Target: %4.3f   %4.3f   %4.3f\n", pos[0], pos[1], pos[2]);
	m_SaggitalViewer.m_EntryPositionMarker.GetCenter( pos );
	printf("Entry: %4.3f   %4.3f   %4.3f\n", pos[0], pos[1], pos[2]);

  float fpos1[3], fpos2[3];
	m_SaggitalViewer.m_EntryToTargetPathMarker.m_LineSource->GetPoint1( fpos1 );
	m_SaggitalViewer.m_EntryToTargetPathMarker.m_LineSource->GetPoint2( fpos2 );
	printf("Line Point 01: %4.3f   %4.3f   %4.3f\n", fpos1[0], fpos1[1], fpos1[2]);
	printf("Line Point 02: %4.3f   %4.3f   %4.3f\n", fpos2[0], fpos2[1], fpos2[2]);
>>>>>>> 1.6
*/
	printf("End of Debug Information:\n");
}

void IGMTracking::RecordReference()
{
	if (m_UseReferenceNeedle)
	{
		m_AuroraTracker.UpdateToolStatus();
		m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ReferenceToolHandle, 
			m_ReferenceNeedleTipOffset, m_ReferenceNeedlePosition);

		m_ForceRecord = true;
	}
}

void IGMTracking::ProcessDicomReading()
{
//<<<<<<< IGMTracking.cxx
	float f;
  
  if (m_VolumeType == 0)
  {
    f = m_DicomVolumeReader.m_Reader->GetProgress();
  }
  else
  {
    f = m_FusionDicomVolumeReader.m_Reader->GetProgress();
  }

	m_ProgressBar->value(f);
	m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();
}

void IGMTracking::ProcessConnectedFilter()
{
	float f;
  
  switch (m_SegParameters.m_SegAlgorithm)
  {
  case 0:
    f = m_ConfidenceConnectedFilter->GetProgress();
    break;
  case 1:
    f = m_ConnectedThresholdFilter->GetProgress();
    break;
  case 2:
    f = m_NeighborhoodConnectedFilter->GetProgress();
    break;
  case 3:
    f = m_IsolatedConnectedFilter->GetProgress();
    break;
  } 

	m_ProgressBar->value(f);
	m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();
}

struct _timeb starttime; 

void IGMTracking::ProcessConnectedFilterStart()
{
  _ftime(&starttime);

	Fl::check();
}

void IGMTracking::ProcessConnectedFilterEnd()
{
  double durtime;
  char mes[128];
  struct _timeb endtime; 

  _ftime(&endtime);
  durtime = endtime.time + endtime.millitm * 0.001 
    - starttime.time - starttime.millitm * 0.001;
  sprintf(mes, "Segmentation Time: %.3fs", durtime);
  this->AppendInfo(mes);

	Fl::check();
}

void IGMTracking::ProcessResampleImagePreFilter()
{
	float f;
  
  f = m_ResampleImagePreFilter->GetProgress();
  
	m_ProgressBar->value(f);
	m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();
}

void IGMTracking::ProcessResampleImagePreFilterStart()
{
  _ftime(&starttime);

	Fl::check();
}

void IGMTracking::ProcessResampleImagePreFilterEnd()
{
  double durtime;
  char mes[128];
  struct _timeb endtime; 

  _ftime(&endtime);
  durtime = endtime.time + endtime.millitm * 0.001 
    - starttime.time - starttime.millitm * 0.001;
  sprintf(mes, "Pre-Resample Time: %.3fs", durtime);
  this->AppendInfo(mes);
//=======
//	double f = m_DicomVolumeReader.m_Reader->GetProgress();
//	printf("%.3f\n", f);
//>>>>>>> 1.6

	Fl::check();
}

void IGMTracking::ProcessResampleImagePostFilter()
{
	float f;
  
  f = m_ResampleImagePostFilter->GetProgress();
  
	m_ProgressBar->value(f);
	m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();
}

void IGMTracking::ProcessResampleImagePostFilterStart()
{
  _ftime(&starttime);

	Fl::check();
}

void IGMTracking::ProcessResampleImagePostFilterEnd()
{
  double durtime;
  char mes[128];
  struct _timeb endtime; 

  _ftime(&endtime);
  durtime = endtime.time + endtime.millitm * 0.001 
    - starttime.time - starttime.millitm * 0.001;
  sprintf(mes, "Post-Resample Time: %.3fs", durtime);
  this->AppendInfo(mes);

	Fl::check();
}

/*
void IGMTracking::SaveAsRaw()
{
	if (!m_VolumeReader->GetOutput())
	{
		return;
	}

	RawWriterType::Pointer writer = RawWriterType::New();
	RawImageIOType::Pointer rawIO = RawImageIOType::New();

	const char * filename = fl_file_chooser("Raw filename", "*.*", "");
	
	if( !filename  || strlen(filename) == 0 )
    {
		return;
    }

	writer->SetFileName(filename);
	writer->SetImageIO(rawIO);
	writer->SetInput(m_DicomVolumeReader.m_Reader->GetOutput());

	writer->Update();
}
*/
void IGMTracking::SetVolumeThreshold(int thres)
{
  m_VolumeViewer.SetThreshold(thres);

  m_VolumeViewer.Render();
}

void IGMTracking::OnLayout()
{
  m_Layout.SetWindowType(m_WindowType);
  m_Layout.SetRenderingMethod(m_RenderingMethod);
  
  m_Layout.ShowModal();

}

void IGMTracking::OnUpdateLayout()
{
  m_WindowType = m_Layout.GetWindowType();
  m_RenderingMethod = m_Layout.GetRenderingMethod();
  m_RenderingCategory = m_Layout.GetRenderingCategory();
  m_FullSizeIdx = m_Layout.GetFullSizeIdx();

  switch (m_FullSizeIdx)
  {
  case -1:
    AxialView->resize(m_OriginalSize[0][0], m_OriginalSize[0][1], m_OriginalSize[0][2], m_OriginalSize[0][3]);
    CoronalView->resize(m_OriginalSize[1][0], m_OriginalSize[1][1], m_OriginalSize[1][2], m_OriginalSize[1][3]);
    SagittalView->resize(m_OriginalSize[2][0], m_OriginalSize[2][1], m_OriginalSize[2][2], m_OriginalSize[2][3]);
    switch (m_WindowType)
    {
    case 0:
      VolumeView->resize(m_OriginalSize[3][0], m_OriginalSize[3][1], m_OriginalSize[3][2], m_OriginalSize[3][3]);
      MotionView->resize(0, 0, 0, 0);
      TargetView->resize(0, 0, 0, 0);
      break;
    case 1:
      VolumeView->resize(0, 0, 0, 0);
      MotionView->resize(m_OriginalSize[3][0], m_OriginalSize[3][1], m_OriginalSize[3][2], m_OriginalSize[3][3]);
      TargetView->resize(0, 0, 0, 0);
      break;
    case 2:
      VolumeView->resize(0, 0, 0, 0);
      MotionView->resize(0, 0, 0, 0);
      TargetView->resize(m_OriginalSize[3][0], m_OriginalSize[3][1], m_OriginalSize[3][2], m_OriginalSize[3][3]);
      break;
    }
    break;
  case 0:
    CoronalView->resize(0, 0, 0, 0);
    SagittalView->resize(0, 0, 0, 0);
    VolumeView->resize(0, 0, 0, 0);
    MotionView->resize(0, 0, 0, 0);
    TargetView->resize(0, 0, 0, 0);
    AxialView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
    break;
  case 1:
    AxialView->resize(0, 0, 0, 0);
    SagittalView->resize(0, 0, 0, 0);
    VolumeView->resize(0, 0, 0, 0);
    MotionView->resize(0, 0, 0, 0);
    TargetView->resize(0, 0, 0, 0);
    CoronalView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
    break;
  case 2:
    CoronalView->resize(0, 0, 0, 0);
    AxialView->resize(0, 0, 0, 0);
    VolumeView->resize(0, 0, 0, 0);
    MotionView->resize(0, 0, 0, 0);
    TargetView->resize(0, 0, 0, 0);
    SagittalView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
    break;
  case 3:
    AxialView->resize(0, 0, 0, 0);
    CoronalView->resize(0, 0, 0, 0);
    SagittalView->resize(0, 0, 0, 0);
    switch (m_WindowType)
    {
    case 0:
      VolumeView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
      MotionView->resize(0, 0, 0, 0);
      TargetView->resize(0, 0, 0, 0);
      break;
    case 1:
      VolumeView->resize(0, 0, 0, 0);
      MotionView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
      TargetView->resize(0, 0, 0, 0);
      break;
    case 2:
      VolumeView->resize(0, 0, 0, 0);
      MotionView->resize(0, 0, 0, 0);
      TargetView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
    }
    break;
  }

  switch (m_WindowType)
  {
  case 0:
    m_VolumeViewer.SetRenderingCategory(m_RenderingCategory);
    m_VolumeViewer.SetRenderingMethod(m_RenderingMethod);
    m_VolumeViewer.ShowFrame(m_Layout.m_ShowFrame);
    m_VolumeViewer.ShowProbe(m_Layout.m_ShowProbe);
    
    if (m_RenderingCategory == 4 && m_DesiredTriNum != m_Layout.m_DesiredTriNum)
    {
      m_DesiredTriNum = m_Layout.m_DesiredTriNum;
      m_VolumeViewer.SetDesiredTriNum(m_DesiredTriNum);
      m_VolumeViewer.m_MarchingCubes->Update();
    }
    m_VolumeViewer.Render();
    break;
  case 1:
    m_MotionViewer.Render();
    break;
  case 2:
    m_TargetViewer.SetupCamera();
    m_TargetViewer.Render();
    break;
  }
  
  if (m_FullSizeIdx != 3)
  {
    m_AxialViewer.ShowAnnotation(m_Layout.m_ShowAnnotation);
    m_AxialViewer.Render();
    m_CoronalViewer.ShowAnnotation(m_Layout.m_ShowAnnotation);
    m_CoronalViewer.Render();
    m_SagittalViewer.ShowAnnotation(m_Layout.m_ShowAnnotation);
    m_SagittalViewer.Render();
  }    
}

void IGMTracking::OnUpdateProbeParameters()
{
  int probe;
  char toolID[10];

  m_UseReferenceNeedle = false;
  
  for (int i = 0; i < PORTNUM; i++)
  {
    m_Probe[i] = m_ProbeParameters.GetDevice(i);

    m_ProbeParameters.GetDeviceLine(i, probe, m_Ref[i], m_UID[i], m_Offset[i], m_Length[i], m_Radius[i]);

    SetProbeLength(i, m_Length[i]);
    SetProbeRadius(i, m_Radius[i]);

    if (probe == 1)
    {
      m_ProbeID = i;
      sprintf(toolID, "%02d", m_ProbeID + 1);
	    m_ToolHandle = m_AuroraTracker.GetMyToolHandle( toolID );
      m_NeedleTipOffset[2] = m_Offset[i];
      m_NeedleEndOffset[2] = m_Length[i];
    }
    else
    {
      m_AxialViewer.DeActivateProbe(i);
      m_CoronalViewer.DeActivateProbe(i);
      m_SagittalViewer.DeActivateProbe(i);
    }

    if (m_ProbeParameters.UseReference() && m_Ref[i] == 1)
    {
      m_RefID = i;
      m_ReferenceNeedleTipOffset[2] = m_Offset[i];
      m_UseReferenceNeedle = true;
    }
  }
}

void IGMTracking::OnFusionImage()
{
	const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
	
	if( !directoryname  || strlen(directoryname) == 0 )
    {
		return;
    }
	
	sprintf(m_StringBuffer, "Directory chosen : %s", directoryname );
	this->PrintMessage(m_StringBuffer);

  m_VolumeType = 1;
	
	m_FusionDicomVolumeReader.SetDirectory( directoryname );
	
  m_FusionDicomVolumeReader.CollectSeriesAndSelectOne();
}

void IGMTracking::OnUpdateOpacity(double opa)
{
  m_ImageBlend->SetOpacity(0, 1.0 - opa);
  m_ImageBlend->SetOpacity(1, opa);

  m_FusionOpacity = opa;
}


void IGMTracking::RenderSectionWindow()
{
  m_AxialViewer.Render();
  m_CoronalViewer.Render();
  m_SagittalViewer.Render();
}

void IGMTracking::SaveSnapshot()
{
  vtkVRMLExporter* exporter = vtkVRMLExporter::New();

  const char* filename = fl_file_chooser("VRML file", "*.wrl", "");

  switch (m_WindowType)
  {
  case 0: // volume
    exporter->SetRenderWindow(m_VolumeViewer.m_RenderWindow);
    break;
  case 1: // motion
    exporter->SetRenderWindow(m_MotionViewer.m_RenderWindow);
    break;
  case 2: // target
    exporter->SetRenderWindow(m_TargetViewer.m_RenderWindow);
    break;
  }
  
  exporter->SetFileName(filename);
  exporter->Update();

  exporter->Delete();
}

void IGMTracking::MCProgressFunc(void *arg)
{
  IGMTracking* tracking = (IGMTracking*)arg;
  float f = tracking->m_VolumeViewer.m_MarchingCubes->GetProgress();

  tracking->m_ProgressBar->value(f);
	tracking->m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();

}

void IGMTracking::DecProgressFunc(void *arg)
{
  IGMTracking* tracking = (IGMTracking*)arg;
  float f = tracking->m_VolumeViewer.m_DecimatePro->GetProgress();

  tracking->m_ProgressBar->value(f);
	tracking->m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();

}

void IGMTracking::SmoothProgressFunc(void *arg)
{
  IGMTracking* tracking = (IGMTracking*)arg;
  float f = tracking->m_VolumeViewer.m_SmoothFilter->GetProgress();

  tracking->m_ProgressBar->value(f);
	tracking->m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();

}

void IGMTracking::OnSegmentation()
{
  unsigned int i;
  double ratio[3];
  VolumeType::IndexType index, index2;
  VolumeType::RegionType region;
  VolumeType::SizeType size, ressize;
  VolumeType::SpacingType spacing, resspacing;
  VolumeType::PointType origin;
  VolumeType::PixelType pixel;
  VolumeType::SizeType radius;
  
  radius[0] = m_SegParameters.m_Radius;
  radius[1] = m_SegParameters.m_Radius;
  radius[2] = m_SegParameters.m_Radius;

  ratio[0] = ratio[1] = m_SegParameters.m_ResolutionXY;
  ratio[2] = m_SegParameters.m_ResolutionZ;

  this->TrackingPoint(m_RightClickedPoint);
  for (i = 0; i < 3; i++)
  {
    index2[i] = m_PointIndex[i];
  }

  pixel = this->TrackingPoint(m_ClickedPoint);

  if (m_PointIndex[0] == 0 && m_PointIndex[1] == 0 && m_PointIndex[2] == 0)
  {
    return;
  }

  if (m_SegParameters.m_MultiResolution == 1)
  {
    region = m_LoadedVolume->GetLargestPossibleRegion();
    size = region.GetSize();
    spacing = m_LoadedVolume->GetSpacing();
    origin = m_LoadedVolume->GetOrigin();
    
    for (i = 0; i < 3; i++)
    {
      index[i] = (int)(m_PointIndex[i] / ratio[i]);
      index2[i] = (int)(index2[i] / ratio[i]);
      ressize[i] = (int)(size[i] / ratio[i]);
      resspacing[i] = spacing[i] * ratio[i];
    }  
    
    m_ResampleImagePreFilter->SetInput(m_LoadedVolume);
    m_ResampleImagePreFilter->SetSize(ressize); 
    m_ResampleImagePreFilter->SetOutputSpacing(resspacing);
    m_ResampleImagePreFilter->SetOutputOrigin(origin);

    switch (m_SegParameters.m_SegAlgorithm)
    {
    case 0:
      m_ConfidenceConnectedFilter->SetSeed(index);
      m_ConfidenceConnectedFilter->SetMultiplier(m_SegParameters.m_Multiplier);
      m_ConfidenceConnectedFilter->SetNumberOfIterations(m_SegParameters.m_Iterations);
      m_ConfidenceConnectedFilter->SetInitialNeighborhoodRadius(m_SegParameters.m_Radius);
      m_ResampleImagePostFilter->SetInput(m_ConfidenceConnectedFilter->GetOutput());
      break;
    case 1:
      m_ConnectedThresholdFilter->SetSeed(index);
      m_ConnectedThresholdFilter->SetLower(pixel + m_SegParameters.m_Lower);
      m_ConnectedThresholdFilter->SetUpper(pixel + m_SegParameters.m_Upper);
      m_ResampleImagePostFilter->SetInput(m_ConnectedThresholdFilter->GetOutput());
      break;
    case 2:
      m_NeighborhoodConnectedFilter->SetSeed(index);
      m_NeighborhoodConnectedFilter->SetLower(pixel + m_SegParameters.m_Lower);
      m_NeighborhoodConnectedFilter->SetUpper(pixel + m_SegParameters.m_Upper);
      m_NeighborhoodConnectedFilter->SetRadius(radius);
      m_ResampleImagePostFilter->SetInput(m_NeighborhoodConnectedFilter->GetOutput());
      break;
    case 3:
      m_IsolatedConnectedFilter->SetSeed1(index);
      m_IsolatedConnectedFilter->SetSeed2(index2);
      m_IsolatedConnectedFilter->SetLower(pixel + m_SegParameters.m_Lower);
      m_ResampleImagePostFilter->SetInput(m_IsolatedConnectedFilter->GetOutput());
      break;
    }

    m_ResampleImagePostFilter->SetSize(size); 
    m_ResampleImagePostFilter->SetOutputSpacing(spacing);
    m_ResampleImagePostFilter->SetOutputOrigin(origin);

    switch (m_SegParameters.m_InterpolatorMethod)
    {
    case 0:
      m_ResampleImagePreFilter->SetInterpolator(m_NNInterpolate);
      m_ResampleImagePostFilter->SetInterpolator(m_NNInterpolate);
      break;
    case 1:
      m_ResampleImagePreFilter->SetInterpolator(m_LInterpolate);
      m_ResampleImagePostFilter->SetInterpolator(m_LInterpolate);
      break;
    }

    m_FusionRescaleIntensity->SetInput( m_ResampleImagePostFilter->GetOutput() );

  }
  else
  {
    for (i = 0; i < 3; i++)
    {
      index[i] = m_PointIndex[i];
    }

    switch (m_SegParameters.m_SegAlgorithm)
    {
    case 0:
      m_ConfidenceConnectedFilter->SetInput(m_LoadedVolume);

      m_ConfidenceConnectedFilter->SetSeed(index);
      m_ConfidenceConnectedFilter->SetMultiplier(m_SegParameters.m_Multiplier);
      m_ConfidenceConnectedFilter->SetNumberOfIterations(m_SegParameters.m_Iterations);
      m_ConfidenceConnectedFilter->SetInitialNeighborhoodRadius(m_SegParameters.m_Radius);
      
      m_FusionRescaleIntensity->SetInput(m_ConfidenceConnectedFilter->GetOutput());
      break;
    case 1:
      m_ConnectedThresholdFilter->SetInput(m_LoadedVolume);

      m_ConnectedThresholdFilter->SetSeed(index);
      m_ConnectedThresholdFilter->SetLower(pixel + m_SegParameters.m_Lower);
      m_ConnectedThresholdFilter->SetUpper(pixel + m_SegParameters.m_Upper);
      
      m_FusionRescaleIntensity->SetInput(m_ConnectedThresholdFilter->GetOutput());
      break;
    case 2:
      m_NeighborhoodConnectedFilter->SetInput(m_LoadedVolume);

      m_NeighborhoodConnectedFilter->SetSeed(index);
      m_NeighborhoodConnectedFilter->SetLower(pixel + m_SegParameters.m_Lower);
      m_NeighborhoodConnectedFilter->SetUpper(pixel + m_SegParameters.m_Upper);
      m_NeighborhoodConnectedFilter->SetRadius(radius);
      
      m_FusionRescaleIntensity->SetInput(m_NeighborhoodConnectedFilter->GetOutput());
      break;
    case 3:
      m_IsolatedConnectedFilter->SetInput(m_LoadedVolume);

      m_IsolatedConnectedFilter->SetSeed1(index);
      m_IsolatedConnectedFilter->SetSeed2(index2);
      m_IsolatedConnectedFilter->SetLower(pixel + m_SegParameters.m_Lower);
      
      m_FusionRescaleIntensity->SetInput(m_IsolatedConnectedFilter->GetOutput());
      break;
    }
  }  

  this->SetInputData(1);

  this->RenderAllWindow();

}

void IGMTracking::SetInputData(int pipeline)
{
  m_Pipeline = pipeline;

  switch (pipeline)
  {
  case 0:
    m_ShiftScaleImageFilter->UpdateWholeExtent();

    m_AxialViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );
    m_CoronalViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );
    m_SagittalViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );
    m_VolumeViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );
    break;
  case 1:
    m_FusionShiftScaleImageFilter->UpdateWholeExtent();

    m_AxialViewer.SetInput( m_FusionShiftScaleImageFilter->GetOutput() );
    m_CoronalViewer.SetInput( m_FusionShiftScaleImageFilter->GetOutput() );
    m_SagittalViewer.SetInput( m_FusionShiftScaleImageFilter->GetOutput() );
    m_VolumeViewer.SetInput( m_FusionShiftScaleImageFilter->GetOutput() );
    break;
  case 2:
    
    m_ImageBlend->Update();

    m_AxialViewer.SetInput( m_ImageBlend->GetOutput() );
    m_CoronalViewer.SetInput( m_ImageBlend->GetOutput() );
    m_SagittalViewer.SetInput( m_ImageBlend->GetOutput() );
    m_VolumeViewer.SetInput( m_ImageBlend->GetOutput() );
    break;
  }
  
}

void IGMTracking::RenderAllWindow()
{
  switch (m_FullSizeIdx)
  {
  case -1:
  case 0:
  case 1:
  case 2:
    this->RenderSectionWindow();
    break;
  case 3:
    break;
  }

  switch (m_WindowType)
  {
  case 0:
    m_VolumeViewer.Render();
    break;
  case 1:
    m_MotionViewer.Render();
    break;
  case 2:
    m_TargetViewer.Render();
    break;
  }
}

void IGMTracking::OnUpdateSegParameters()
{

}

void IGMTracking::OnSaveSegmentationData()
{
	if (!m_FusionRescaleIntensity->GetInput())
	{
		return;
	}

	RawWriterType::Pointer writer = RawWriterType::New();
	RawImageIOType::Pointer rawIO = RawImageIOType::New();

	const char * filename = fl_file_chooser("Raw filename", "*.*", "");
	
	if( !filename  || strlen(filename) == 0 )
    {
		return;
    }

	writer->SetFileName(filename);
	writer->SetImageIO(rawIO);
	writer->SetInput(m_FusionRescaleIntensity->GetInput());

	writer->Update();
}
