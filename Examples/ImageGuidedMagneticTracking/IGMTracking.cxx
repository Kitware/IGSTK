
#include "IGMTracking.h"

#include "FL/Fl_File_Chooser.H"

#include "FL/Fl_Text_Display.h"

#include "vtkImageShiftScale.h"
#include "vtkPolyData.h"
#include "vtkVRMLExporter.h"

#include "ClickedPointEvent.h"

#include "Conversions.h"

#include "DICOMFile.h"

#include <stdlib.h>
#include <conio.h>
#include "io.h"
#include "SYS\TIMEB.H"
#include ".\igmtracking.h"

bool	continueTracking;

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

IGMTracking::IGMTracking()
: m_InRecord(false)
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

  m_InvertFilter = InvertFilterType::New();
  m_InvertFilter->SetLowerThreshold(250);
  m_InvertFilter->SetUpperThreshold(260);
  m_InvertFilter->SetOutsideValue(255);
  m_InvertFilter->SetInsideValue(0);

  m_DistanceMapFilter = DistanceFilterType::New();

  m_DistanceMapProgressCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_DistanceMapProgressCommand->SetCallbackFunction(this, IGMTracking::ProcessDistanceMapFilter);
  m_DistanceMapFilter->AddObserver(itk::ProgressEvent(), m_DistanceMapProgressCommand);

	m_Overlay = false;
	m_ShowVolume = false; //false;//
	m_EntryPointSelected = false; 
	m_TargetPointSelected = false;
	m_ForceRecord = false;

  m_FullSizeIdx = -1;
  m_WindowType = 0;
  m_RenderingMethod = 0;
  m_RenderingCategory = 0;
  m_DesiredTriNum = 30000;
  m_Pipeline = 0;

  m_ProbeID = 0;
  m_RegisterID = 0;
  
  m_Probe[0] = 1;
  m_Register[0] = 1;
  m_Ref[0] = 0;
  m_UID[0] = 1042;
  m_Offset[0] = -12.6;
  m_Length[0] = 100.0;
  m_Radius[0] = 1.5;

  m_Probe[1] = 0;
  m_Register[1] = 0;
  m_Ref[1] = 1;
  m_UID[1] = 1027;
  m_Offset[1] = -19.1;
  m_Length[1] = 100.0;
  m_Radius[1] = 1.5;

  m_Probe[2] = 0;
  m_Register[2] = 0;
  m_Ref[2] = 0;
  m_UID[2] = 0;
  m_Offset[2] = 0.0;
  m_Length[2] = 10.0;
  m_Radius[2] = 1.0;

  m_Probe[3] = 0;
  m_Register[3] = 0;
  m_Ref[3] = 0;
  m_UID[3] = 0;
  m_Offset[3] = 0.0;
  m_Length[3] = 10.0;
  m_Radius[3] = 1.0;

  m_RegNeedleTipOffset[0] = 0.0;
  m_RegNeedleTipOffset[1] = 0.0;
  m_RegNeedleTipOffset[2] = -12.6;
  
  m_CoilOffset1[0] = m_CoilOffset1[1] = 0;
  m_CoilOffset1[2] = 4.5;
  m_CoilOffset2[0] = m_CoilOffset2[1] = 0;
  m_CoilOffset2[2] = -4.5;

  int i;

  for (i = 0; i < 4; i++)
  {
    m_4NeedleOffset[i][0] = 0;
    m_4NeedleOffset[i][1] = 0;
    m_4NeedleOffset[i][2] = 0;
  }

  for (i = 0; i < 8; i++)
  {
    m_8CoilOffset[i][0] = 0;
    m_8CoilOffset[i][1] = 0;
    m_8CoilOffset[i][2] = 0;
  }

  for (i = 0; i < PORTNUM; i++)
  {
    m_ProbeParameters.SetDeviceLine(i, m_Probe[i], m_Register[i], m_Ref[i], m_UID[i], m_Offset[i], m_Length[i], m_Radius[i]);
  }

	this->SetNeedleLength( this->GetNeedleLength() );
	this->SetNeedleRadius( this->GetNeedleRadius() );

  m_VolumeType = 0;

  for (i = 0; i < 3; i++)
  {
    m_PointIndex[i] = 0;
  }  

  m_MovingPointSet = PointSetType::New();

  m_CATransform = IGSTK::FantasticRegistration::CATransformType::New() ;

  m_E3DTransform = IGSTK::FantasticRegistration::E3DTransformType::New() ;

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
 /* 
  unsigned int i;
  
  IGSTK::FantasticRegistration::PointsContainerType::Pointer pc = IGSTK::FantasticRegistration::PointsContainerType::New();
  IGSTK::FantasticRegistration::PointSetType::PointType point;
  unsigned int it = 0;

  for (i = 0; i < 15; i++)
  {
    point[0] = point[1] = 5.0;
    point[2] = i * 0.1 + 0.1;

    pc->InsertElement(it, point);
      it++;
  }

  this->m_FantasticRegistration.m_FixedPointSet->SetPoints(pc);

  it = 0;
  pc = IGSTK::FantasticRegistration::PointsContainerType::New();

  for (i = 0; i < 25; i++)
  {
    point[0] = 4.0;
    point[1] = 5.0;
    point[2] = i * 0.1 + 0.1;

    pc->InsertElement(it, point);
      it++;

  }

  this->m_FantasticRegistration.m_MovingPointSet->SetPoints(pc);
  m_FantasticRegistration.StartRegistration();
 
  IGSTK::FantasticRegistration::E3DParametersType para = m_FantasticRegistration.m_PointSetToPointSetRegistration->GetLastTransformParameters();
//  m_TTransform->SetParameters(para);

  double p[15];

  for (i = 0; i < 15; i++)
  {
    p[i] = para[i];
  }

  double s1 = m_FantasticRegistration.m_LMOptimizer->GetOptimizer()->get_start_error();
  double s2 = m_FantasticRegistration.m_LMOptimizer->GetOptimizer()->get_end_error();
*/
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

  /*

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
  */

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
		m_AxialViewer.ActivateProbe(m_ProbeID);
		m_CoronalViewer.ActivateProbe(m_ProbeID);
		m_SagittalViewer.ActivateProbe(m_ProbeID);
	}
	else
	{
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
  double pos[3];
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
  double pos[3], dis, d[3];
  char mes[128];
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
    
    dis = 0;
    for (i = 0; i < 3; i++)
    {
      d[i] = pTracking->m_RightClickedPoint[i] - pTracking->m_ClickedPoint[i];
      dis += d[i] * d[i];
    }
    sprintf(mes, "%.3fmm", sqrt(dis));
    pTracking->m_DistancePane->value(mes);
  }  

}

void IGMTracking::ProcessCoronalViewMouseMoveInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  double pos[3];
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
  double pos[3], dis, d[3];
  char mes[128];

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

    dis = 0;
    for (i = 0; i < 3; i++)
    {
      d[i] = pTracking->m_RightClickedPoint[i] - pTracking->m_ClickedPoint[i];
      dis += d[i] * d[i];
    }
    sprintf(mes, "%.3fmm", sqrt(dis));
    pTracking->m_DistancePane->value(mes);
  } 
}

void IGMTracking::ProcessSagittalViewMouseMoveInteraction(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  double pos[3];
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
  double pos[3], dis, d[3];
  char mes[128];

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

    dis = 0;
    for (i = 0; i < 3; i++)
    {
      d[i] = pTracking->m_RightClickedPoint[i] - pTracking->m_ClickedPoint[i];
      dis += d[i] * d[i];
    }
    sprintf(mes, "%.3fmm", sqrt(dis));
    pTracking->m_DistancePane->value(mes);
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

int IGMTracking::TrackingPoint( const double aboutPoint[3], bool show )
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

  m_FantasticRegistration.AddFixedPoint(point);
}


	
void IGMTracking::SetTrackerFiducial( unsigned int fiducialNumber )
{
  unsigned int i;

	IGMTrackingBase::SetTrackerFiducial( fiducialNumber, m_NeedlePosition );
  
  switch (m_RegParameters.m_RegType)
  {
  case 0:
    sprintf(m_StringBuffer, "Tracker Fiducials %d set at ( %4.3f, %4.3f, %4.3f)", 
		  fiducialNumber + 1, m_NeedlePosition[0], m_NeedlePosition[1], m_NeedlePosition[2] );
	  this->PrintMessage(m_StringBuffer);  
    break;
  case 1:
    for (i = 0; i < 4; i++)
    {
      sprintf(m_StringBuffer, "Tracker Fiducials %d set at ( %4.3f, %4.3f, %4.3f)", 
		    i, m_CoilPosition[i][0], m_CoilPosition[i][1], m_CoilPosition[i][2] );
	    this->PrintMessage(m_StringBuffer);  
    }
    break;
  case 2:
    break;
  }
	
}

	
void IGMTracking::InitializeTracker( const char *fileName )
{
	if (!m_AuroraTracker.InitializeTracker( fileName ))
	{
		this->PrintMessage("Initialization of Aurora Failed...");
		this->AppendInfo("Initialization of Aurora Failed...");
	}
	else
	{
		this->PrintMessage("Initialization Aurora Succeeded...");
		this->AppendInfo("Initialization of Aurora Succeeded...");
	}
}


void IGMTracking::ActivateTools( void )
{
	if (!m_AuroraTracker.ActivateTools())
	{
		this->PrintMessage("Activation of Aurora Tools Failed...");
    this->AppendInfo("Activation of Aurora Tools Failed...");
	}
	else
	{
		this->PrintMessage("Activation of Aurora Tools Succeeded...");
    this->AppendInfo("Activation of Aurora Tools Succeeded...");
	}
}


void IGMTracking::StartTracking( void )
{	
  int i, ret, tool1, tool2, tool;
  char toolID[10];
  char mes[128];

  m_AuroraTracker.StartTracking();

  for (i = 0; i < 4; i++)
  {
    if (m_Probe[i] == 1)
    {
      switch (m_RegParameters.m_RegType)
      {
      case 0:
      case 3:
      case 4:
        sprintf(toolID, "%02d", i + 1);
	      tool = m_AuroraTracker.GetMyToolHandle( toolID );
        sprintf(mes, "Tool on port %d recognised ...", i);
        this->AppendInfo(mes);
        if (m_RegisterID == i)
        {
          m_RegToolHandle = tool;
        }
        break;
      case 1:
        sprintf(toolID, "%02d", i + 1);
        ret = m_AuroraTracker.GetMyToolHandle2(toolID, tool1, tool2);
        if (ret < 2)
        {
          sprintf(mes, "Not two tool on port %d detected ...", i);          
        }
        else
        {
          sprintf(mes, "Two tools on port %d recognised ...", i);
        }
        this->AppendInfo(mes);
        if (m_RegisterID == i)
        {
          m_RegToolHandle1 = tool1;
          m_RegToolHandle2 = tool2;
        }
        break;
      case 2:
        sprintf(toolID, "%02d", i + 1);
	      tool = m_AuroraTracker.GetMyToolHandle( toolID );        
        sprintf(mes, "Tool on port %d recognised ...", i);
        this->AppendInfo(mes);
        if (m_RegisterID == i)
        {
          m_RegToolHandle = tool;
        }
        break;
      }      
    }      
  }
  
  sprintf(toolID, "%02d", m_ProbeID + 1);
	m_ToolHandle = m_AuroraTracker.GetMyToolHandle( toolID );
  sprintf(mes, "Probe Tool on port %d recognised ...", m_ProbeID);
  this->AppendInfo(mes);

  if (m_UseReferenceNeedle)
	{
    sprintf(toolID, "%02d", m_RefID + 1 );
		m_ReferenceToolHandle = m_AuroraTracker.GetMyToolHandle( toolID );
    printf("Ref:%d handle:%d\n", m_RefID, m_ReferenceToolHandle);
		if (m_ReferenceToolHandle<0)
		{
      sprintf(mes, "No reference tool on port %d detected ...", m_RefID);
		}
		else
		{
      sprintf(mes, "Rreference tool on port %d recognised ...", m_RefID);
		}
    this->AppendInfo(mes);
	}

  for (i = 0; i < 4; i++)
  {
    sprintf(toolID, "%02d", i + 1);
	  m_4NeedleRegToolHandle[i] = m_AuroraTracker.GetMyToolHandle( toolID );
    sprintf(toolID, "%02d", i + 1);
    ret = m_AuroraTracker.GetMyToolHandle2(toolID, tool1, tool2);
    if (ret == 2)
    {
      m_8CoilRegToolHandle[i * 2] = tool1;
      m_8CoilRegToolHandle[i * 2 + 1] = tool2;
    }
  }

  m_InRecord = false;

}

void IGMTracking::StopTracking( void )
{
	m_AuroraTracker.StopTracking();
  continueTracking = false;
  if (m_RegParameters.m_RFAPosition == 0)
  {
    this->DisplayToolPosition(false);
  }
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

	
void IGMTracking::OnRegisterPosition( void )
{
  unsigned int i;
  m_AuroraTracker.UpdateToolStatus();

  switch (m_RegParameters.m_RegType)
  {
  case 0:
    m_AuroraTracker.GetOffsetCorrectedToolPosition( m_RegToolHandle, m_RegNeedleTipOffset, m_NeedlePosition);
    printf("register: %.3f %.3f %.3f\n", 
      m_NeedlePosition[0], m_NeedlePosition[1], m_NeedlePosition[2]);
    break;
  case 1:
    m_AuroraTracker.GetOffsetCorrectedToolPosition( m_RegToolHandle1, m_CoilOffset1, m_CoilPosition[0]);
    m_AuroraTracker.GetOffsetCorrectedToolPosition( m_RegToolHandle1, m_CoilOffset2, m_CoilPosition[1]);
    m_AuroraTracker.GetOffsetCorrectedToolPosition( m_RegToolHandle2, m_CoilOffset1, m_CoilPosition[2]);
    m_AuroraTracker.GetOffsetCorrectedToolPosition( m_RegToolHandle2, m_CoilOffset2, m_CoilPosition[3]);
    for (i = 0; i < 4; i++)
    {
      printf("register %d: %.3f %.3f %.3f\n", i,
        m_CoilPosition[i][0], m_CoilPosition[i][1], m_CoilPosition[i][2]);
    }
    break;
  case 2:
    break;
  case 3:
    for (i = 0; i < 4; i++)
    {
      m_AuroraTracker.GetOffsetCorrectedToolPosition( m_4NeedleRegToolHandle[i], m_4NeedleOffset[i], m_4NeedlePosition[i]);
      printf("register %d: %.3f %.3f %.3f\n", i, 
        m_4NeedlePosition[i][0], m_4NeedlePosition[i][1], m_4NeedlePosition[i][2]);
    }    
    break;
  case 4:
    for (i = 0; i < 8; i++)
    {
      m_AuroraTracker.GetOffsetCorrectedToolPosition( m_8CoilRegToolHandle[i], m_8CoilOffset[i], m_8CoilPosition[i]);
      printf("register %d: %.3f %.3f %.3f\n", i, 
        m_8CoilPosition[i][0], m_8CoilPosition[i][1], m_8CoilPosition[i][2]);
    } 
    break;
  }	
	this->PrintToolPosition( m_ToolHandle ); 
}

void IGMTracking::OnProbePosition( void )
{
//  unsigned int i;
	m_AuroraTracker.UpdateToolStatus();
/*
  switch (m_RegParameters.m_RegType)
  {
  case 0:
  case 1:
  case 2:
    m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ToolHandle, m_RegNeedleTipOffset, m_NeedlePosition);
    break;
  case 3:
    for (i = 0; i < 4; i++)
    {
      m_AuroraTracker.GetOffsetCorrectedToolPosition( m_4NeedleRegToolHandle[i], m_4NeedleOffset[i], m_4NeedlePosition[i]);
    }    
    break;
  case 4:
    for (i = 0; i < 8; i++)
    {
      m_AuroraTracker.GetOffsetCorrectedToolPosition( m_8CoilRegToolHandle[i], m_8CoilOffset[i], m_8CoilPosition[i]);
    } 
    break;
  }	
*/
	m_AuroraTracker.UpdateToolStatus();
  m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ToolHandle, m_NeedleTipOffset, m_NeedlePosition);  	
	this->PrintToolPosition( m_ToolHandle ); 
}

void IGMTracking::DumpSkeletonInfo( void )
{
  unsigned int i, num = m_Skeleton.m_Skeleton->GetNumberOfPoints();
  char filename[256], tmpbuf[128], mes[128];
  SkeletonModuleType::PointSetType::PointType point;
  
  _strtime( tmpbuf );
  
  for (i = 0; i < strlen(tmpbuf); i++)
  {
    if (tmpbuf[i] == ':')
    {
      tmpbuf[i] = ' ';
    }
  }
  
  sprintf(filename, "skeleton%s.dat", tmpbuf);

  FILE * fp;  
  fp = fopen(filename, "w+");

  for (i = 0; i < num; i++)
  {
    m_Skeleton.m_Skeleton->GetPoint(i, &point);
    sprintf(mes, " %4.3f %4.3f %4.3f\n", point[0], point[1], point[2]);
    if (fp)
    {
      fprintf(fp, mes);
    }
  }

  fclose(fp);
}

void IGMTracking::DumpRegisterMatrix( char* filename)
{
  unsigned int i, j;
  char mes[128];

  FILE* fp = fopen(filename, "w+");

  double element;
  vtkMatrix4x4* matrix;
  IGSTK::FantasticRegistration::CAParametersType para;

  sprintf(mes, "Reg=%d\n", m_RegParameters.m_RegType);
  if (fp)
  {
    fprintf(fp, mes);
  }

  switch (m_RegParameters.m_RegType)
  {
  case 0:
  case 1:
  case 3:
  case 4:
    matrix = this->m_FiducialRegistration.m_LandmarkTransform->GetMatrix();
    for (j = 0; j < 4; j++)
    {
      sprintf(mes, "");
      for (i = 0; i < 4; i++)
      {
        element = matrix->GetElement(i, j);
        sprintf(mes, "%s %4.3f ", mes, element);
      }
      sprintf(mes, "%s\n", mes);
      if (fp)
      {
        fprintf(fp, mes);
      }
    }
    break;
  case 2:
    sprintf(mes, "");
    para = m_FantasticRegistration.m_PointSetToPointSetRegistration->GetLastTransformParameters();
    for (i = 0; i < m_FantasticRegistration.m_PointSetToPointSetRegistration->GetTransform()->GetNumberOfParameters(); i++)
    {
      sprintf(mes, "%s %4.3f\n", mes, para[i]);
    }
    if (fp)
    {
      fprintf(fp, mes);
    }
    break;
  }

  if (fp)
  {
    fclose(fp);
  }
}

void IGMTracking::DumpRegisterMatrix( void )
{
  unsigned int i;
  char filename[256], tmpbuf[128];
  _strtime( tmpbuf );
  for (i = 0; i < strlen(tmpbuf); i++)
  {
    if (tmpbuf[i] == ':')
    {
      tmpbuf[i] = ' ';
    }
  }
  sprintf(filename, "matrix%s.dat", tmpbuf);

  this->DumpRegisterMatrix(filename); 
}

void IGMTracking::DumpRegisterInfo( void )
{
  unsigned int i;
  char filename[256], tmpbuf[128], mes[128];
  _strtime( tmpbuf );
  for (i = 0; i < strlen(tmpbuf); i++)
  {
    if (tmpbuf[i] == ':')
    {
      tmpbuf[i] = ' ';
    }
  }
  sprintf(filename, "register%s.dat", tmpbuf);

  vtkPoints* points;
  float* p;

  IGSTK::FantasticRegistration::PointSetType::Pointer pointset;
  IGSTK::FantasticRegistration::PointType point;

  FILE * fp;  
  fp = fopen(filename, "w+");

  switch (m_RegParameters.m_RegType)
  {
  case 0:
  case 1:
  case 3:
  case 4:
    points = m_FiducialRegistration.m_LandmarkTransform->GetSourceLandmarks();

    for (i = 0; i < points->GetNumberOfPoints(); i++)
    {
      p = points->GetPoint(i);
      sprintf(mes, "S-P%d: %4.3f %4.3f %4.3f\n", i, *p, *(p + 1), *(p + 2));
      if (fp)
      {
        fprintf(fp, mes);
      }
    }

    points = m_FiducialRegistration.m_LandmarkTransform->GetTargetLandmarks();

    for (i = 0; i < points->GetNumberOfPoints(); i++)
    {
      p = points->GetPoint(i);
      sprintf(mes, "D-P%d: %4.3f %4.3f %4.3f\n", i, *p, *(p + 1), *(p + 2));
      if (fp)
      {
        fprintf(fp, mes);
      }
    }
    break;
  case 2:
    pointset = (IGSTK::FantasticRegistration::PointSetType*)m_FantasticRegistration.m_PointSetToPointSetRegistration->GetFixedPointSet();
    for (i = 0; i < pointset->GetNumberOfPoints(); i++)
    {
      pointset->GetPoint(i, &point);
      sprintf(mes, "S-P%d: %4.3f %4.3f %4.3f\n", i, point[0], point[1], point[2]);
      if (fp)
      {
        fprintf(fp, mes);
      }
    }

    pointset = (IGSTK::FantasticRegistration::PointSetType*)m_FantasticRegistration.m_PointSetToPointSetRegistration->GetMovingPointSet();
    for (i = 0; i < pointset->GetNumberOfPoints(); i++)
    {
      pointset->GetPoint(i, &point);
      sprintf(mes, "D-P%d: %4.3f %4.3f %4.3f\n", i, point[0], point[1], point[2]);
      if (fp)
      {
        fprintf(fp, mes);
      }
    }
    break;
  } 

  if (fp)
  {
    fclose(fp);
  }

}

bool IGMTracking::OnRegister( void )
{
  unsigned int i, j, k, k1, idx[4], minidx[4], id;
  double err, starterr, minerr = 100000;
  unsigned int num1, num2;
  bool ret = true, reg = false;
  char mes[128];
  
  IGSTK::FantasticRegistration::CAParametersType para;  

  switch (m_RegParameters.m_RegType)
  {
  case 0: // points pair landmark
    ret = this->RegisterImageWithTrackerAndComputeRMSError();
    this->AppendInfo(m_StringBuffer);
    this->DumpRegisterInfo();
    break;
  case 1: // dual sensor cathetor 
    id = 0;
    for (i = 0; i < 2 && !reg; i++)
    {
      idx[0] = i;
      idx[1] = 1 - i;
      for (j = 2; j < 4 && !reg; j++)
      {
        idx[2] = j;
        idx[3] = 5 - j;
        for (k = 0; k < 4; k++)
        {
          m_FiducialRegistration.SetSourceFiducial(k, m_CoilPosition[idx[k]]);
        }
        this->RegisterImageWithTrackerAndComputeRMSError();
        err = m_FiducialRegistration.GetRMSError();
        sprintf(mes, "The %drd registration error:%.2f", id, err);
        this->AppendInfo(mes);
        if (err < minerr)
        {
          memcpy(minidx, idx, 4 * sizeof(unsigned int));
          minerr = err;
        }
        if (id == m_RegParameters.m_DualSensorRegNum)
        {
          reg = true;
          this->DumpRegisterInfo();
        }
        id++;
      }      
    }

    for (i = 2; i < 4 && !reg; i++)
    {
      idx[0] = i;
      idx[1] = 5 - i;
      for (j = 0; j < 2 && !reg; j++)
      {
        idx[2] = j;
        idx[3] = 1 - j;
        for (k = 0; k < 4; k++)
        {
          m_FiducialRegistration.SetSourceFiducial(k, m_CoilPosition[idx[k]]);
        }
        this->RegisterImageWithTrackerAndComputeRMSError();
        err = m_FiducialRegistration.GetRMSError();
        sprintf(mes, "The %drd registration error:%.2f", id, err);
        this->AppendInfo(mes);
        if (err < minerr)
        {
          memcpy(minidx, idx, 4 * sizeof(unsigned int));
          minerr = err;
        }
        if (id == m_RegParameters.m_DualSensorRegNum)
        {
          reg = true;
          this->DumpRegisterInfo();
        }
        id++;
      }      
    }
    
    if (m_RegParameters.m_DualSensorRegNum == -1)
    {
      for (k = 0; k < 4; k++)
      {
        m_FiducialRegistration.SetSourceFiducial(k, m_CoilPosition[minidx[k]]);
      }
      this->RegisterImageWithTrackerAndComputeRMSError();      
      err = m_FiducialRegistration.GetRMSError();
      sprintf(mes, "The final registration error:%.2f", err);
      this->AppendInfo(mes);
      this->DumpRegisterInfo();
    }
    break;
  case 2: // vascular registration
    m_FantasticRegistration.SetFixedPointSet(m_Skeleton.m_Skeleton);
    if (m_RegParameters.m_SimplifyFixed == 1)
    {
      m_FantasticRegistration.SimplifyFixedPointSet(m_RegParameters.m_Radius);
    }
    m_FantasticRegistration.SetMovingPointSet(m_MovingPointSet);
    if (m_RegParameters.m_SimplifyMoving == 1)
    {
      m_FantasticRegistration.SimplifyMovingPointSet(m_RegParameters.m_Radius);
    }    
    m_FantasticRegistration.StartRegistration();
    para = m_FantasticRegistration.m_PointSetToPointSetRegistration->GetLastTransformParameters();
    m_E3DTransform->SetParameters(para);
    num1 = m_FantasticRegistration.m_FixedPointSet->GetNumberOfPoints();
    num2 = m_FantasticRegistration.m_MovingPointSet->GetNumberOfPoints();
    starterr = m_FantasticRegistration.m_LMOptimizer->GetOptimizer()->get_start_error();
    err = m_FantasticRegistration.m_LMOptimizer->GetOptimizer()->get_end_error();
    sprintf(mes, "Fixed/Moving points number:%d %d", num1, num2);
    this->AppendInfo(mes);
    sprintf(mes, "Start/end RMS error:%.3f %.3f", starterr, err);
    this->AppendInfo(mes);
    this->DumpRegisterInfo();
    break;
  case 3:
/*    for (k = 0; k < 4; k++)
    {
      m_FiducialRegistration.SetSourceFiducial(k, m_4NeedlePosition[k]);
    }
*/    
    for (k = 0, k1 = 8; k < 4; k++)
    {
      k1 -= m_RegParameters.m_Needle[k];
    }
    for (k = 0; k < 4; k++ )
    {
      if (m_RegParameters.m_Needle[k] == 1)
      {
        m_FiducialRegistration.SetSourceFiducial(k1, m_4NeedlePosition[k]);
        k1++;
      }
    }
    ret = this->RegisterImageWithTrackerAndComputeRMSError();
    this->AppendInfo(m_StringBuffer);
    this->DumpRegisterInfo();
    break;
  case 4:
    for (k = 0; k < 8; k++)
    {
      m_FiducialRegistration.SetSourceFiducial(k, m_8CoilPosition[k]);
    }
    ret = this->RegisterImageWithTrackerAndComputeRMSError();
    this->AppendInfo(m_StringBuffer);
    this->DumpRegisterInfo();
    break;
  }  

  return true;
}

/*
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
*/
void IGMTracking::TransformPoint(double* in, double* out)
{
  unsigned int i;
  IGSTK::FantasticRegistration::E3DTransformType::InputPointType inputpoint, outputpoint;

  for (i = 0; i < 3; i++)
  {
    inputpoint[i] = in[i];
  }
  outputpoint = m_E3DTransform->TransformPoint(inputpoint);
  for (i = 0; i < 3; i++)
  {
    out[i] = outputpoint[i];
  }
}
/*
void OverlayTracking(void* p)
{
  IGMTracking* pTracking = (IGMTracking*)p;
  static   int run = 0;
  unsigned int i;
  bool firstpos = true;
  double HomePos[3], d[3], dis, motiondis;
  double toolPositionInImageSpace[3], anotherPositionInImageSpace[3];
  float spacing[3];
  double anotherPosition[3];
  double NoRefPosition1[3], NoRefPosition2[3];
  double refToolNewPosition[3];
  float offset[3];
  double offsetInImageSpace1[3], offsetInImageSpace2[3];
  double tippos[3], hippos[3];
  char mes[128];

  IGMTracking::PointSetType::PointType recordpoint;

  FILE * fp;
  fp = fopen("overlay_track_position.dat", "a+");

  sprintf(pTracking->m_StringBuffer, "***********   OVERLAY RUN %.3d DATA   *********\n", ++run );	
  if (fp != NULL) fprintf(fp, pTracking->m_StringBuffer);
  printf ( pTracking->m_StringBuffer );

  if (pTracking->m_UseReferenceNeedle && !pTracking->m_ForceRecord)
  {
    pTracking->m_AuroraTracker.UpdateToolStatus();
    pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ReferenceToolHandle, 
      pTracking->m_ReferenceNeedleTipOffset, pTracking->m_ReferenceNeedlePosition);

    for (i = 0; i < 3; i++)
    {
      HomePos[i] = pTracking->m_ReferenceNeedlePosition[i];
    }
  }

  pTracking->m_Overlay = true;
  continueTracking = true;
  pTracking->DisplayToolPosition( true );

  while (continueTracking)
  {
    printf("loop\n");
    pTracking->OnProbePosition();
    pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ToolHandle, pTracking->m_NeedleEndOffset, anotherPosition);

    if (pTracking->m_UseReferenceNeedle)
    {
      pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ReferenceToolHandle, 
        pTracking->m_ReferenceNeedleTipOffset, refToolNewPosition);

      for (i=0; i<3; i++)
      {
        offset[i] = refToolNewPosition[i] - pTracking->m_ReferenceNeedlePosition[i];
        NoRefPosition1[i] = pTracking->m_NeedlePosition[i];
        pTracking->m_NeedlePosition[i] -= offset[i];
        NoRefPosition2[i] = anotherPosition[i];
        anotherPosition[i] -= offset[i];
      }

      if (pTracking->m_WindowType == 1)
      {
        for (i = 0; i < 3; i++)
        {
          d[i] = HomePos[i] - refToolNewPosition[i];
        }

        motiondis = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        pTracking->m_MotionViewer.UpdateMotion(motiondis);
      }
    }

    switch (pTracking->m_RegParameters.m_RegType)
    {
    case 0:
    case 1:
      pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(pTracking->m_NeedlePosition,toolPositionInImageSpace);
      pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(anotherPosition,anotherPositionInImageSpace);
      break;
    case 2:
      pTracking->TransformPoint(pTracking->m_NeedlePosition, toolPositionInImageSpace);
      pTracking->TransformPoint(anotherPosition, anotherPositionInImageSpace);
      break;
    }			

    sprintf(mes, "(%.2f %.2f %.2f)", 
      toolPositionInImageSpace[0], toolPositionInImageSpace[1], toolPositionInImageSpace[2]);
    pTracking->m_ProbePosPane->value(mes);

    pTracking->m_ShiftScaleImageFilter->GetOutput()->GetSpacing(spacing);

    dis = 0;
    for (i = 0; i < 3; i++)
    {
      d[i] = toolPositionInImageSpace[i] - pTracking->m_ClickedPoint[i];
      dis += d[i] * d[i];
    }
    sprintf(mes, "%.3fmm", sqrt(dis));
    pTracking->m_DistancePane->value(mes);

    if (pTracking->m_FullSizeIdx != 3)
    {
      pTracking->m_AxialViewer.DeActivateSelectedPosition();
      pTracking->m_CoronalViewer.DeActivateSelectedPosition();
      pTracking->m_SagittalViewer.DeActivateSelectedPosition();

      pTracking->m_AxialViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_AxialViewer.MakeToolPositionMarkerVisible();

      pTracking->m_CoronalViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_CoronalViewer.MakeToolPositionMarkerVisible();

      pTracking->m_SagittalViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_SagittalViewer.MakeToolPositionMarkerVisible();

      //pTracking->SyncAllViews( toolPositionInImageSpace );
    }

    if (pTracking->m_WindowType == 0 && pTracking->m_VolumeViewer.GetProbeVisibility() == 1)
    {
      pTracking->m_VolumeViewer.SetProbeTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_VolumeViewer.Render();
    }

    if (pTracking->m_WindowType == 2)
    {
      if (pTracking->m_UseReferenceNeedle)
      {
        switch (pTracking->m_RegParameters.m_RegType)
        {
        case 0:
        case 1:
          pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(NoRefPosition1, toolPositionInImageSpace);
          pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(NoRefPosition2, anotherPositionInImageSpace);
          pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(refToolNewPosition, offsetInImageSpace2);
          pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(pTracking->m_ReferenceNeedlePosition, offsetInImageSpace1);
          break;
        case 2:
          pTracking->TransformPoint(NoRefPosition1, toolPositionInImageSpace);
          pTracking->TransformPoint(NoRefPosition2, anotherPositionInImageSpace);
          pTracking->TransformPoint(refToolNewPosition, offsetInImageSpace2);
          pTracking->TransformPoint(pTracking->m_ReferenceNeedlePosition, offsetInImageSpace1);                
          break;
        }

        for (i = 0; i < 3; i++)
        {
          tippos[i] = toolPositionInImageSpace[i];
          hippos[i] = anotherPositionInImageSpace[i];
        }
        pTracking->m_TargetViewer.SetProbePosition(tippos, hippos);
        pTracking->m_TargetViewer.TranslateTargetPoint(offsetInImageSpace2[0] - offsetInImageSpace1[0], 
          offsetInImageSpace2[1] - offsetInImageSpace1[1], 
          offsetInImageSpace2[2] - offsetInImageSpace1[2]);
        pTracking->m_TargetViewer.Render();
      }  
      else
      {
        for (i = 0; i < 3; i++)
        {
          tippos[i] = toolPositionInImageSpace[i];
          hippos[i] = anotherPositionInImageSpace[i];
        }
        pTracking->m_TargetViewer.SetProbePosition(tippos, hippos);
        pTracking->m_TargetViewer.Render();
      }      
    }     

    sprintf(pTracking->m_StringBuffer, "%4.3f   %4.3f   %4.3f\n", toolPositionInImageSpace[0], toolPositionInImageSpace[1], toolPositionInImageSpace[2]);
    if (fp != NULL) fprintf(fp, pTracking->m_StringBuffer);	  	

    Sleep(10);
  }

  if (fp!=NULL) 
  {
    fprintf(fp, "\n\n\n\n\n");
    fclose(fp);
  }

  _endthread();
}
*/
void OverlayTracking(void *p)
{
  _getch();
  continueTracking = false;    /* _endthread implied */
}

void IGMTracking::OnOverlay( void )
{
  IGMTracking* pTracking = this;
  static   int run = 0;
  unsigned int i;
  bool firstpos = true;
//  double HomePos[3];
  double d[3], dis, motiondis;
  double toolPositionInImageSpace[3], anotherPositionInImageSpace[3];
  float spacing[3];
  double anotherPosition[3];
  double NoRefPosition1[3], NoRefPosition2[3];
  double refToolNewPosition[3];
  float offset[3];
  double offsetInImageSpace1[3], offsetInImageSpace2[3];
  double tippos[3], hippos[3];
  char mes[128];

  IGMTracking::PointSetType::PointType recordpoint;

  char filename[256], tmpbuf[128];;
  _strtime( tmpbuf );
  for (i = 0; i < strlen(tmpbuf); i++)
  {
    if (tmpbuf[i] == ':')
    {
      tmpbuf[i] = ' ';
    }
  }
  sprintf(filename, "overlay%s.dat", tmpbuf);

  FILE * fp;
  fp = fopen(filename, "w+");

  double tmpPosition[3], tmpQuad[4];

  if (pTracking->m_UseReferenceNeedle && !pTracking->m_ForceRecord 
    && pTracking->m_ReferenceToolHandle > 0)
  {
    pTracking->m_AuroraTracker.UpdateToolStatus();    

 //   for (i = 0; i < 3; i++)
//    {
  //    HomePos[i] = pTracking->m_ReferenceNeedlePosition[i];
//    }

    pTracking->m_AuroraTracker.GetToolPosition( pTracking->m_ReferenceToolHandle, 
      tmpPosition);
/*    sprintf(mes, "InitialRefPosition: %4.3f %4.3f %4.3f\n",
      tmpPosition[0], tmpPosition[1], tmpPosition[2]);
    if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
    if (fp != NULL) 
    {
      fprintf(fp, mes);
    }
*/
    pTracking->m_AuroraTracker.GetToolOrientation( pTracking->m_ReferenceToolHandle, 
      tmpQuad);
/*    sprintf(mes, "InitialRefOrientation: %4.3f %4.3f %4.3f %4.3f\n",
      tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
    if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
    if (fp != NULL) 
    {
      fprintf(fp, mes);
    }
*/
    pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ReferenceToolHandle, 
      pTracking->m_ReferenceNeedleTipOffset, pTracking->m_ReferenceNeedlePosition);
/*    sprintf(mes, "InitialOffsetedRef: %4.3f %4.3f %4.3f\n",
      pTracking->m_ReferenceNeedlePosition[0], pTracking->m_ReferenceNeedlePosition[1], pTracking->m_ReferenceNeedlePosition[2]);
    if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
    if (fp != NULL) 
    {
      fprintf(fp, mes);
    }
    */
  }

  pTracking->m_Overlay = true;
  continueTracking = true;
  pTracking->DisplayToolPosition( true );

  if (_beginthread(OverlayTracking, 0, NULL) != -1)
  {
    this->PrintMessage("Thread Successfully Created.");
  }
  else
  {
    this->PrintMessage("Thread Creation Failed.");
  }

  while (continueTracking)
  {
    pTracking->OnProbePosition();
/*
    pTracking->m_AuroraTracker.GetToolPosition( pTracking->m_ToolHandle, 
      tmpPosition);
    sprintf(mes, "P-Tip Position: %4.3f %4.3f %4.3f\n",
      tmpPosition[0], tmpPosition[1], tmpPosition[2]);
    if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
    if (fp != NULL) 
    {
      fprintf(fp, mes);
    }

    pTracking->m_AuroraTracker.GetToolOrientation( pTracking->m_ToolHandle, 
      tmpQuad);
    sprintf(mes, "P-Tip Orientation: %4.3f %4.3f %4.3f %4.3f\n",
      tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
    if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
    if (fp != NULL) 
    {
      fprintf(fp, mes);
    }
*/    
    pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ToolHandle, pTracking->m_NeedleTipOffset, pTracking->m_NeedlePosition);
/*    sprintf(mes, "P-Tip: %4.3f %4.3f %4.3f\n", 
      pTracking->m_NeedlePosition[0], pTracking->m_NeedlePosition[1], pTracking->m_NeedlePosition[2]);
    if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
    if (fp != NULL)
    { 
      fprintf(fp, mes);
    }
*/
    pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ToolHandle, pTracking->m_NeedleEndOffset, anotherPosition);
/*    sprintf(mes, "P-Hip: %4.3f %4.3f %4.3f\n", 
      anotherPosition[0], anotherPosition[1], anotherPosition[2]);
    if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
    if (fp != NULL)
    { 
      fprintf(fp, mes);
    }
*/
    if (pTracking->m_UseReferenceNeedle && pTracking->m_ReferenceToolHandle > 0)
    {
/*      pTracking->m_AuroraTracker.GetToolPosition( pTracking->m_ReferenceToolHandle, 
        tmpPosition);
      sprintf(mes, "RefPosition: %4.3f %4.3f %4.3f\n",
        tmpPosition[0], tmpPosition[1], tmpPosition[2]);
      if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }

      pTracking->m_AuroraTracker.GetToolOrientation( pTracking->m_ReferenceToolHandle, 
        tmpQuad);
      sprintf(mes, "RefOrientation: %4.3f %4.3f %4.3f %4.3f\n",
        tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
      if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }
*/
      pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ReferenceToolHandle, 
        pTracking->m_ReferenceNeedleTipOffset, refToolNewPosition);

  /*    sprintf(mes, "Ref: %4.3f %4.3f %4.3f\n", 
        refToolNewPosition[0], refToolNewPosition[1], refToolNewPosition[2]);
      if (strlen(mes) > 100)
    {
      sprintf(mes, "");
    }
      if (fp != NULL)
      { 
        fprintf(fp, mes);
      }
*/
      for (i=0; i<3; i++)
      {
        offset[i] = refToolNewPosition[i] - pTracking->m_ReferenceNeedlePosition[i];
        NoRefPosition1[i] = pTracking->m_NeedlePosition[i];
        pTracking->m_NeedlePosition[i] -= offset[i];
        NoRefPosition2[i] = anotherPosition[i];
        anotherPosition[i] -= offset[i];
      }

      if (pTracking->m_WindowType == 1)
      {
        for (i = 0; i < 3; i++)
        {
          d[i] = pTracking->m_ReferenceNeedlePosition[i] - refToolNewPosition[i];
        }

        motiondis = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        pTracking->m_MotionViewer.UpdateMotion(motiondis);
      }
    }

    switch (pTracking->m_RegParameters.m_RegType)
    {
    case 0:
    case 1:
    case 3:
    case 4:
      pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(pTracking->m_NeedlePosition,toolPositionInImageSpace);
      pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(anotherPosition,anotherPositionInImageSpace);
      break;    
    case 2:
      pTracking->TransformPoint(pTracking->m_NeedlePosition, toolPositionInImageSpace);
      pTracking->TransformPoint(anotherPosition, anotherPositionInImageSpace);
      break;
    }			

    sprintf(mes, "(%.2f %.2f %.2f)", 
      toolPositionInImageSpace[0], toolPositionInImageSpace[1], toolPositionInImageSpace[2]);
    pTracking->m_ProbePosPane->value(mes);

    pTracking->m_ShiftScaleImageFilter->GetOutput()->GetSpacing(spacing);

    dis = 0;
    for (i = 0; i < 3; i++)
    {
      d[i] = toolPositionInImageSpace[i] - pTracking->m_ClickedPoint[i];
      dis += d[i] * d[i];
    }
    sprintf(mes, "%.3fmm\n", sqrt(dis));
    pTracking->m_DistancePane->value(mes);

    sprintf(mes, "");

    if (pTracking->m_FullSizeIdx != 3)
    {
      pTracking->m_AxialViewer.DeActivateSelectedPosition();
      pTracking->m_CoronalViewer.DeActivateSelectedPosition();
      pTracking->m_SagittalViewer.DeActivateSelectedPosition();

      pTracking->m_AxialViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_AxialViewer.MakeToolPositionMarkerVisible();

      pTracking->m_CoronalViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_CoronalViewer.MakeToolPositionMarkerVisible();

      pTracking->m_SagittalViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_SagittalViewer.MakeToolPositionMarkerVisible();

      pTracking->SyncAllViews( toolPositionInImageSpace );
    }

    if (pTracking->m_WindowType == 0 && pTracking->m_VolumeViewer.GetProbeVisibility() == 1)
    {
      pTracking->m_VolumeViewer.SetProbeTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_VolumeViewer.Render();
    }

    if (pTracking->m_WindowType == 2)
    {
      if (pTracking->m_UseReferenceNeedle && pTracking->m_ReferenceToolHandle > 0)
      {
        switch (pTracking->m_RegParameters.m_RegType)
        {
        case 0:
        case 1:
        case 3:
        case 4:
          pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(NoRefPosition1, toolPositionInImageSpace);
          pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(NoRefPosition2, anotherPositionInImageSpace);
          pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(refToolNewPosition, offsetInImageSpace2);
          pTracking->m_FiducialRegistration.FromSourceSpaceToDestinationSpace(pTracking->m_ReferenceNeedlePosition, offsetInImageSpace1);
          break;
        case 2:
          pTracking->TransformPoint(NoRefPosition1, toolPositionInImageSpace);
          pTracking->TransformPoint(NoRefPosition2, anotherPositionInImageSpace);
          pTracking->TransformPoint(refToolNewPosition, offsetInImageSpace2);
          pTracking->TransformPoint(pTracking->m_ReferenceNeedlePosition, offsetInImageSpace1);                
          break;
        }

        for (i = 0; i < 3; i++)
        {
          tippos[i] = toolPositionInImageSpace[i];
          hippos[i] = anotherPositionInImageSpace[i];
        }
        pTracking->m_TargetViewer.SetProbePosition(tippos, hippos);
        pTracking->m_TargetViewer.TranslateTargetPoint(offsetInImageSpace2[0] - offsetInImageSpace1[0], 
          offsetInImageSpace2[1] - offsetInImageSpace1[1], 
          offsetInImageSpace2[2] - offsetInImageSpace1[2]);
        pTracking->m_TargetViewer.Render();
      }  
      else
      {
        for (i = 0; i < 3; i++)
        {
          tippos[i] = toolPositionInImageSpace[i];
          hippos[i] = anotherPositionInImageSpace[i];
        }
        pTracking->m_TargetViewer.SetProbePosition(tippos, hippos);
        pTracking->m_TargetViewer.Render();
      }      
    }     

    Sleep(10);
  }

  if (fp!=NULL) 
  {
    fprintf(fp, "\n\n\n\n\n");
    fclose(fp);
  }
}

double IGMTracking::GetImageScale( void )
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
}
	
void IGMTracking::SetNeedleRadius( const double val )
{
	IGMTrackingBase::SetNeedleRadius( val );
}

void IGMTracking::SetProbeLength( int i, const float val )
{
  m_AxialViewer.m_ProbeMarker[i].SetLength(val);
  m_CoronalViewer.m_ProbeMarker[i].SetLength(val);
  m_SagittalViewer.m_ProbeMarker[i].SetLength(val);
}
	
void IGMTracking::SetProbeRadius( int i, const float val )
{
	m_AxialViewer.m_ProbeMarker[i].SetRadius(val);
	m_CoronalViewer.m_ProbeMarker[i].SetRadius(val);
	m_SagittalViewer.m_ProbeMarker[i].SetRadius(val);
}	

void IGMTracking::PrintDebugInfo( void )
{
	double pos1[3];
	printf("Debug Information:\n");
	m_SagittalViewer.m_TargetPositionMarker.GetCenter( pos1 );
	printf("Target: %4.3f   %4.3f   %4.3f\n", pos1[0], pos1[1], pos1[2]);
	m_SagittalViewer.m_EntryPositionMarker.GetCenter( pos1 );
	printf("Entry: %4.3f   %4.3f   %4.3f\n", pos1[0], pos1[1], pos1[2]);
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


void IGMTracking::ProcessDistanceMapFilter()
{
	float f;
  
  f = m_DistanceMapFilter->GetProgress();
  
	m_ProgressBar->value(f);
	m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();
}

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
    AxialView->set_visible_focus();
    break;
  case 1:
    AxialView->resize(0, 0, 0, 0);
    SagittalView->resize(0, 0, 0, 0);
    VolumeView->resize(0, 0, 0, 0);
    MotionView->resize(0, 0, 0, 0);
    TargetView->resize(0, 0, 0, 0);
    CoronalView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
    CoronalView->set_visible_focus();
    break;
  case 2:
    CoronalView->resize(0, 0, 0, 0);
    AxialView->resize(0, 0, 0, 0);
    VolumeView->resize(0, 0, 0, 0);
    MotionView->resize(0, 0, 0, 0);
    TargetView->resize(0, 0, 0, 0);
    SagittalView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
    SagittalView->set_visible_focus();
    break;
  case 3:
    AxialView->resize(0, 0, 0, 0);
    CoronalView->resize(0, 0, 0, 0);
    SagittalView->resize(0, 0, 0, 0);
    switch (m_WindowType)
    {
    case 0:
      VolumeView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
      VolumeView->set_visible_focus();
      MotionView->resize(0, 0, 0, 0);
      TargetView->resize(0, 0, 0, 0);
      break;
    case 1:
      VolumeView->resize(0, 0, 0, 0);
      MotionView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
      MotionView->set_visible_focus();
      TargetView->resize(0, 0, 0, 0);
      break;
    case 2:
      VolumeView->resize(0, 0, 0, 0);
      MotionView->resize(0, 0, 0, 0);
      TargetView->resize(m_FullSize[0], m_FullSize[1], m_FullSize[2], m_FullSize[3]);
      TargetView->set_visible_focus();
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

    m_ProbeParameters.GetDeviceLine(i, probe, m_Register[i], m_Ref[i], m_UID[i], m_Offset[i], m_Length[i], m_Radius[i]);

    SetProbeLength(i, m_Length[i]);
    SetProbeRadius(i, m_Radius[i]);

    m_4NeedleOffset[i][2] = m_Offset[i];

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

    if (m_Register[i] == 1)
    {
      m_RegisterID = i;
      m_RegNeedleTipOffset[2] = m_Offset[i];
      m_RegNeedleEndOffset[2] = m_Length[i];
      m_CoilOffset1[2] = m_Offset[i];
      m_CoilOffset2[2] = -m_Offset[i];
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

void IGMTracking::OnUpdateRegParameters()
{
  m_Skeleton.SetClusterRadius(m_RegParameters.m_Radius);

  IGSTK::FantasticRegistration::LMOptimizerType::ScalesType scales( m_E3DTransform->GetNumberOfParameters() );
  scales.Fill( m_RegParameters.m_Scales );
  
  unsigned long   numberOfIterations =  m_RegParameters.m_Iterations;
  double          gradientTolerance  =  m_RegParameters.m_Gradient;   // convergence criterion
  double          valueTolerance     =  m_RegParameters.m_Value;   // convergence criterion
  double          epsilonFunction    =  m_RegParameters.m_Epsilon;   // convergence criterion

  m_FantasticRegistration.m_LMOptimizer->SetScales( scales );
  m_FantasticRegistration.m_LMOptimizer->SetNumberOfIterations( numberOfIterations );
  m_FantasticRegistration.m_LMOptimizer->SetValueTolerance( valueTolerance );
  m_FantasticRegistration.m_LMOptimizer->SetGradientTolerance( gradientTolerance );
  m_FantasticRegistration.m_LMOptimizer->SetEpsilonFunction( epsilonFunction );

}

void IGMTracking::OnSkeleton()
{
  m_Skeleton.SetInput(m_FusionRescaleIntensity->GetInput());
  m_Skeleton.SetProgressCallback(IGMTracking::ProcessSkeleton, this);
  m_Skeleton.ExtractSkeleton();

  m_FusionRescaleIntensity->SetInput(m_Skeleton.GetOutput());
  m_FusionRescaleIntensity->Update();

  this->DumpSkeletonInfo();

//  m_VolumeViewer.SetFixedPointRadius(m_RegParameters.m_Radius);
//  m_VolumeViewer.SetFixedPointSet(m_Skeleton.m_Skeleton);

  this->RenderSectionWindow();
  
/*
  m_InvertFilter->SetInput(m_FusionRescaleIntensity->GetInput());
  m_DistanceMapFilter->SetInput(m_InvertFilter->GetOutput());
  m_FusionRescaleIntensity->SetInput(m_DistanceMapFilter->GetOutput());
  m_FusionRescaleIntensity->Update();
*/
}

void IGMTracking::ProcessSkeleton(void* calldata, double f)
{
  IGMTracking* pTracking = (IGMTracking*)calldata;
  
  pTracking->m_ProgressBar->value(f);
	pTracking->m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();
}

void RecordTracking(void* p)
{
  IGMTracking* pTracking = (IGMTracking*) p;

  static   int run = 0;
  unsigned int i, j;
	double anotherPosition[3];
  double NoRefPosition1[3], NoRefPosition2[3];
  double refToolNewPosition[3];
  float offset[3];
 
  IGMTracking::PointSetType::PointType recordpoint;

  int h[2], n;
  char toolID[128];
  sprintf(toolID, "%02d", pTracking->m_RefID + 1);
  n = pTracking->m_AuroraTracker.GetMyToolHandle2(toolID, h[0], h[1]);

  char filename[256], tmpbuf[128], mes[128];
  _strtime( tmpbuf );
  for (i = 0; i < strlen(tmpbuf); i++)
  {
    if (tmpbuf[i] == ':')
    {
      tmpbuf[i] = ' ';
    }
  }
  sprintf(filename, "record%s.dat", tmpbuf);

  FILE* fp = fopen(filename, "w+");

  double tmpPosition[3], tmpQuad[4];

  if (pTracking->m_UseReferenceNeedle && pTracking->m_ReferenceToolHandle > 0)
	{
		pTracking->m_AuroraTracker.UpdateToolStatus();

    pTracking->m_AuroraTracker.GetToolPosition(pTracking->m_ReferenceToolHandle, tmpPosition);
    sprintf(mes, "InitialRefPosition: %4.3f %4.3f %4.3f\n",
      tmpPosition[0], tmpPosition[1], tmpPosition[2]);
    if (fp != NULL) 
    {
      fprintf(fp, mes);
    }

    pTracking->m_AuroraTracker.GetToolOrientation(pTracking->m_ReferenceToolHandle, tmpQuad);
    sprintf(mes, "InitialRefOrientation: %4.3f %4.3f %4.3f %4.3f\n",
      tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
    if (fp != NULL) 
    {
      fprintf(fp, mes);
    }

    pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ReferenceToolHandle, 
			pTracking->m_ReferenceNeedleTipOffset, pTracking->m_ReferenceNeedlePosition);   
    sprintf(mes, "InitialRefOffsetedPosition: %4.3f %4.3f %4.3f\n",
      pTracking->m_ReferenceNeedlePosition[0], pTracking->m_ReferenceNeedlePosition[1], pTracking->m_ReferenceNeedlePosition[2]);
    if (fp != NULL) 
    {
      fprintf(fp, mes);
    }
	}

  while (pTracking->m_InRecord)
  {
  	pTracking->OnProbePosition();		

    switch (pTracking->m_RegParameters.m_RegType)
    {
    case 0:
    case 1:
      pTracking->m_AuroraTracker.GetToolPosition(pTracking->m_ToolHandle, tmpPosition);
      sprintf(mes, "P-Tip-Position: %4.3f %4.3f %4.3f\n",
        tmpPosition[0], tmpPosition[1], tmpPosition[2]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }
      
      pTracking->m_AuroraTracker.GetToolOrientation(pTracking->m_ToolHandle, tmpQuad);
      sprintf(mes, "P-Tip-Orientation: %4.3f %4.3f %4.3f %4.3f\n",
        tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }
      
      pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ToolHandle, 
        pTracking->m_NeedleTipOffset, pTracking->m_NeedlePosition);
      sprintf(mes, "P-Tip: %4.3f %4.3f %4.3f\n", 
        pTracking->m_NeedlePosition[0], pTracking->m_NeedlePosition[1], pTracking->m_NeedlePosition[2]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }

      printf(mes);
      
      pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ToolHandle, 
        pTracking->m_NeedleEndOffset, anotherPosition);
      sprintf(mes, "P-Hip: %4.3f %4.3f %4.3f\n",
        anotherPosition[0], anotherPosition[1], anotherPosition[2]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }
      break;
    case 2:
      pTracking->m_AuroraTracker.GetToolPosition(pTracking->m_ToolHandle, tmpPosition);
      sprintf(mes, "P-Tip-Position: %4.3f %4.3f %4.3f\n",
        tmpPosition[0], tmpPosition[1], tmpPosition[2]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }
      
      pTracking->m_AuroraTracker.GetToolOrientation(pTracking->m_ToolHandle, tmpQuad);
      sprintf(mes, "P-Tip-Orientation: %4.3f %4.3f %4.3f %4.3f\n",
        tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }
      
      pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ToolHandle, 
        pTracking->m_NeedleTipOffset, pTracking->m_NeedlePosition);
      sprintf(mes, "P-Tip: %4.3f %4.3f %4.3f\n", 
        pTracking->m_NeedlePosition[0], pTracking->m_NeedlePosition[1], pTracking->m_NeedlePosition[2]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }

      printf(mes);
      
      pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ToolHandle, 
        pTracking->m_NeedleEndOffset, anotherPosition);
      sprintf(mes, "P-Hip: %4.3f %4.3f %4.3f\n",
        anotherPosition[0], anotherPosition[1], anotherPosition[2]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }

      if (n == 2)
      {
        for (j = 0; j < 2; j++)
        {
          pTracking->m_AuroraTracker.GetToolPosition(h[j], tmpPosition);
          sprintf(mes, "R-Tip-Position%d: %4.3f %4.3f %4.3f\n",
            j, tmpPosition[0], tmpPosition[1], tmpPosition[2]);
          if (fp != NULL) 
          {
            fprintf(fp, mes);
          }
          
          pTracking->m_AuroraTracker.GetToolOrientation(h[j], tmpQuad);
          sprintf(mes, "R-Tip-Orientation%d: %4.3f %4.3f %4.3f %4.3f\n",
            j, tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
          if (fp != NULL) 
          {
            fprintf(fp, mes);
          }
        }
      }
      break;
    case 3:
      for ( i = 0; i < 4; i++)
      {
        pTracking->m_AuroraTracker.GetToolPosition(pTracking->m_4NeedleRegToolHandle[i], tmpPosition);
        sprintf(mes, "P-Tip-Position%d: %4.3f %4.3f %4.3f\n",
          i, tmpPosition[0], tmpPosition[1], tmpPosition[2]);
        if (fp != NULL) 
        {
          fprintf(fp, mes);
        }
        
        pTracking->m_AuroraTracker.GetToolOrientation(pTracking->m_4NeedleRegToolHandle[i], tmpQuad);
        sprintf(mes, "P-Tip-Orientation%d: %4.3f %4.3f %4.3f %4.3f\n",
          i, tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
        if (fp != NULL) 
        {
          fprintf(fp, mes);
        }

        pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_4NeedleRegToolHandle[i], 
          pTracking->m_4NeedleOffset[i], pTracking->m_4NeedlePosition[i]);
        sprintf(mes, "P-Tip%d: %4.3f %4.3f %4.3f\n", 
          i, pTracking->m_4NeedlePosition[i][0], pTracking->m_4NeedlePosition[i][1], pTracking->m_4NeedlePosition[i][2]);
        if (fp != NULL) 
        {
          fprintf(fp, mes);
        }
      }
      break;
    case 4:
      for ( i = 0; i < 8; i++)
      {
        pTracking->m_AuroraTracker.GetToolPosition(pTracking->m_8CoilRegToolHandle[i], tmpPosition);
        sprintf(mes, "P-Tip-Position%d: %4.3f %4.3f %4.3f\n",
          i, tmpPosition[0], tmpPosition[1], tmpPosition[2]);
        if (fp != NULL) 
        {
          fprintf(fp, mes);
        }
        
        pTracking->m_AuroraTracker.GetToolOrientation(pTracking->m_8CoilRegToolHandle[i], tmpQuad);
        sprintf(mes, "P-Tip-Orientation%d: %4.3f %4.3f %4.3f %4.3f\n",
          i, tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
        if (fp != NULL) 
        {
          fprintf(fp, mes);
        }
        
        pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_8CoilRegToolHandle[i], 
          pTracking->m_8CoilOffset[i], pTracking->m_8CoilPosition[i]);
        sprintf(mes, "P-Tip%d: %4.3f %4.3f %4.3f\n", 
          i, pTracking->m_8CoilPosition[i][0], pTracking->m_8CoilPosition[i][1], pTracking->m_8CoilPosition[i][2]);
        if (fp != NULL) 
        {
          fprintf(fp, mes);
        }        
      }
      break;
    }
    

    if (pTracking->m_UseReferenceNeedle && pTracking->m_ReferenceToolHandle > 0)
		{
      pTracking->m_AuroraTracker.GetToolPosition(pTracking->m_ReferenceToolHandle, tmpPosition);
      sprintf(mes, "RefPosition: %4.3f %4.3f %4.3f\n",
        tmpPosition[0], tmpPosition[1], tmpPosition[2]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }

      pTracking->m_AuroraTracker.GetToolOrientation(pTracking->m_ReferenceToolHandle, tmpQuad);
      sprintf(mes, "RefOrientation: %4.3f %4.3f %4.3f %4.3f\n",
        tmpQuad[0], tmpQuad[1], tmpQuad[2], tmpQuad[3]);
      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }

		  pTracking->m_AuroraTracker.GetOffsetCorrectedToolPosition( pTracking->m_ReferenceToolHandle, 
			  pTracking->m_ReferenceNeedleTipOffset, refToolNewPosition);
      sprintf(mes, "Ref: %4.3f %4.3f %4.3f\n",
        refToolNewPosition[0], refToolNewPosition[1], refToolNewPosition[2]);

      if (fp != NULL) 
      {
        fprintf(fp, mes);
      }
				
			for (i = 0; i < 3; i++)
			{
				offset[i] = refToolNewPosition[i] - pTracking->m_ReferenceNeedlePosition[i];
        NoRefPosition1[i] = pTracking->m_NeedlePosition[i];
				pTracking->m_NeedlePosition[i] -= offset[i];
        NoRefPosition2[i] = anotherPosition[i];
				anotherPosition[i] -= offset[i];
			}		
		}
			
    for (i = 0; i < 3; i++)
    {
      recordpoint[i] = pTracking->m_NeedlePosition[i];
    }
      
    pTracking->m_MovingPointSet->SetPoint(pTracking->m_MovingPointSet->GetNumberOfPoints(), recordpoint);

    Sleep(10); 
    
  }

  if (fp != NULL)
  {
    fclose(fp);
  }

  _endthread();

}

void IGMTracking::OnRecord(void)
{
  m_InRecord = !m_InRecord;

  if (m_InRecord)
  {
    if (_beginthread(RecordTracking, 0, (void*) this) != -1)
    {
      this->PrintMessage("Thread Successfully Created.");
    }
    else
    {
      this->PrintMessage("Thread Creation Failed.");
    }
  }
}

void IGMTracking::OnLoadMatrix(void)
{
  const char * filename = fl_file_chooser("Matrix file","*.dat","");

  if( !filename  || strlen(filename) == 0 )
  {
    return;
  }

  int regtype;
  unsigned int i, j;

  printf("dfsdf");

  FILE *fp = fopen(filename, "r");

  vtkMatrix4x4* matrix;
  float element;
  IGSTK::FantasticRegistration::CAParametersType para;

  fscanf(fp, "Reg=%d", &regtype);
  switch (regtype)
  {
  case 0:
  case 1:
  case 3:
  case 4:
    matrix = this->m_FiducialRegistration.m_LandmarkTransform->GetMatrix();
    for (j = 0; j < 4; j++)
    {
      for (i = 0; i < 4; i++)
      {
        fscanf(fp, "%f", &element);
        matrix->SetElement(i, j, element);
      }
    }
    break;
  case 2:
    for (i = 0; i < m_FantasticRegistration.m_PointSetToPointSetRegistration->GetTransform()->GetNumberOfParameters(); i++)
    {
      fscanf(fp, "%f", &element);
      para[i] = element;
    }
    m_E3DTransform->SetParameters(para);
    break;
  }

  if (fp)
  {
    fclose(fp);
  }

}

void IGMTracking::OnSaveMatrix(void)
{
  char * filename = fl_file_chooser("Matrix file","*.dat","");

  if( !filename  || strlen(filename) == 0 )
  {
    return;
  }

  this->DumpRegisterMatrix(filename);
}

void IGMTracking::OnLoadPET(void)
{
  const char * directoryname = fl_dir_chooser("PET Image directory","");
	
	if( !directoryname  || strlen(directoryname) == 0 )
  {
		return;
  }
	
  m_VolumeType = 0;
	
	std::vector<std::string> filename;
  std::vector<std::string>::iterator it;


	
	char name[1024];
	sprintf(name, "%s", directoryname);
	char* buf;
	int n = static_cast<int>( strlen(name) );
	if ( name[n - 1] == '/' ) 
  {
		buf = new char[n + 1 + 1];
		sprintf(buf, "%s*", name);
  } 
	else
  {
		buf = new char[n + 2 + 1];
		sprintf(buf, "%s/*", name);
  }
	
	struct _finddata_t data;
  
	long srchHandle = _findfirst(buf, &data);
	delete [] buf;
  
  if ( srchHandle == -1 )
  {
    return ;
  }
    
  char s[256];
  FILE* file;
  CDICOMFile dicom;
  dicom.SetSwap(true);
  double spacing[3], position[3], sliceloc;
  unsigned int size[3];
  unsigned int i, j, slice = 0;
  
  do 
  {
    if (strcmp(data.name, ".") != 0 && strcmp(data.name, "..") != 0)
    {
      sprintf(s, "%s/%s", directoryname, data.name);
		  filename.push_back(s);         
		}
  } 
	while ( _findnext(srchHandle, &data) != -1 );
	_findclose(srchHandle);  


  int num;
  signed short *buffer, *buffer2;
  double *slicelocation;
  it = filename.begin();
  do 
  {
    file = fopen(it->c_str(), "rb+");
    dicom.Open(file);
    dicom.SwapImage();
    fclose(file);
    if (slice == 0)
    {
      for (i = 0; i < 3; i++)
      {
        position[i] = dicom.m_Position[i];
      }
      for (i = 0; i < 2; i++)
      {
        spacing[i] = dicom.m_Spacing[i];
      }
      sliceloc = dicom.m_SliceLocation;        
      size[0] = dicom.m_CX;
      size[1] = dicom.m_CY;
      size[2] = filename.size();
      num = size[0] * size[1] * size[2];
      buffer = new signed short[num];
      slicelocation = new double[size[2]];
    }
    else if (slice == 1)
    {
      spacing[2] = fabs(dicom.m_SliceLocation - sliceloc);
    }
    slicelocation[slice] = dicom.m_SliceLocation;
    memcpy(buffer + slice * size[0] * size[1], dicom.m_pImage, size[0] * size[1] * sizeof(signed short));
    slice++;
    it++;
  }
  while (it != filename.end());

  int* index = new int[size[2]], temp;
  for (i = 0; i< size[2]; i++)
  {
    index[i] = i;
  }
  for (i = 0; i < size[2] - 1; i++)
  {
    for (j = i + 1; j < size[2]; j++)
    {
      if (slicelocation[index[i]] < slicelocation[index[j]])
      {
        temp = index[i];
        index[i] = index[j];
        index[j] = temp;
      }
    }
  }

  spacing[2] = fabs(slicelocation[index[1]] - slicelocation[index[0]]);

  buffer2 = new signed short[num];
  for (i = 0; i < size[2]; i++)
  {
    memcpy(buffer2 + i * size[0] * size[1], buffer + index[i] * size[0] * size[1], size[0] * size[1] * sizeof(signed short));
  }

  delete buffer;

  typedef itk::ImportImageFilter<signed short, 3> ImportFilterType;

  typedef ImportFilterType::Pointer ImportFilterTypePointer;

  typedef itk::Image<signed short, 3>::RegionType InputRegionType;

  typedef itk::Image<signed short, 3>::SizeType InputSizeType;

  typedef itk::Image<signed short, 3>::IndexType InputIndexType;

  ImportFilterTypePointer m_pImportFilter = ImportFilterType::New();

  InputRegionType region;

  InputSizeType ssize;

  InputIndexType iindex;

  for (i = 0; i < 3; i++)
  {
    ssize[i] = size[i];
    iindex[i] = 0;
  }
  region.SetIndex(iindex);
  region.SetSize(ssize);  

  m_pImportFilter->SetRegion(region);
  m_pImportFilter->SetSpacing(spacing);
  m_pImportFilter->SetOrigin(position);

  m_pImportFilter->SetImportPointer(buffer2, num, false);

  m_LoadedVolume = m_pImportFilter->GetOutput();
	
//	m_Reader->SetFileNames(filename);	

//  m_VolumeIsLoaded = true;

  this->LoadPostProcessing();

//  m_Notifier->InvokeEvent( itk::EndEvent() );
}

void IGMTracking::OnLoadPath(void)
{
  unsigned int i;
  char mes[128];
  float point[4];
  std::ifstream pathfile;
  std::string str;
  IGMTracking::PointSetType::PointType pathpoint;

  char * filename = fl_file_chooser("Matrix file","*.dat","");

  if( !filename  || strlen(filename) == 0 )
  {
    return;
  }

  pathfile.open(filename);
  if (pathfile.fail())
  {
    return;
  }

  i = 0;
  while( !pathfile.eof() )
  {
    pathfile>>str;
    if (str == "P-Tip:")
    {
      pathfile>>pathpoint;
      i++;
   //   if (i < 50)
   //   {
        m_MovingPointSet->SetPoint(m_MovingPointSet->GetNumberOfPoints(), pathpoint);
   //   }
      
    }
    else if (str == "P-Tip-Orientation:" || str == "R-Tip-Orientation0:" || str == "R-Tip-Orientation1:")
    {
      pathfile>>point[0]>>point[1]>>point[2]>>point[3];
    }
    else 
    {
      pathfile>>point[0]>>point[1]>>point[2];
    }    
  }

  sprintf(mes, "%d path points loaded!", i);
  this->AppendInfo(mes);

}

void IGMTracking::OnSimulatePath(void)
{
  unsigned int i, j;
  double toolPositionInImageSpace[3], anotherPositionInImageSpace[3], pos1[3], pos2[3];
  IGMTracking* pTracking = this;

  IGSTK::FantasticRegistration::PointType point;

  pTracking->DisplayToolPosition( true );

  for (i = 0; i < pTracking->m_MovingPointSet->GetNumberOfPoints(); i++)
  {
    pTracking->m_MovingPointSet->GetPoint(i, &point);
    
    for (j = 0; j < 3; j++)
    {
      pos1[j] = point[j];
      pos2[j] = point[j] + 10;
    }

    switch (pTracking->m_RegParameters.m_RegType)
    {
    case 0:
    case 1:
    case 3:
    case 4:
      break;    
    case 2:
      pTracking->TransformPoint(pos1, toolPositionInImageSpace);
      pTracking->TransformPoint(pos2, anotherPositionInImageSpace);
      break;
    }		
    
    if (pTracking->m_FullSizeIdx != 3)
    {
      pTracking->m_AxialViewer.DeActivateSelectedPosition();
      pTracking->m_CoronalViewer.DeActivateSelectedPosition();
      pTracking->m_SagittalViewer.DeActivateSelectedPosition();

      pTracking->m_AxialViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_AxialViewer.MakeToolPositionMarkerVisible();

      pTracking->m_CoronalViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_CoronalViewer.MakeToolPositionMarkerVisible();

      pTracking->m_SagittalViewer.SetProbeTipAndDirection(pTracking->m_ProbeID, toolPositionInImageSpace,anotherPositionInImageSpace);
      pTracking->m_SagittalViewer.MakeToolPositionMarkerVisible();

      pTracking->SyncAllViews( toolPositionInImageSpace );
    }	

    Sleep(100);
  }
  
  
}
