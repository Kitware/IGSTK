
#include "IGMTracking.h"

#include "FL/Fl_File_Chooser.H"

#include "FL/Fl_Text_Display.h"

#include "vtkImageShiftScale.h"

#include "ClickedPointEvent.h"

#include "Conversions.h"

#include <stdlib.h>
#include <conio.h>

IGMTracking::IGMTracking()
{
	m_AxialViewer.SetOrientation( ISIS::ImageSliceViewer::Axial    );
	m_CoronalViewer.SetOrientation( ISIS::ImageSliceViewer::Coronal    );
	m_SaggitalViewer.SetOrientation( ISIS::ImageSliceViewer::Saggital    );
	
	m_ShiftScaleImageFilter = vtkImageShiftScale::New();
	
	m_ShiftScaleImageFilter->SetInput(  m_ITK2VTKAdaptor->GetOutput() );
	m_ShiftScaleImageFilter->SetShift( 15 ); // 0
	m_ShiftScaleImageFilter->SetScale( 2.0 ); //1.5
	m_ShiftScaleImageFilter->SetOutputScalarTypeToUnsignedChar();
	m_ShiftScaleImageFilter->ClampOverflowOn();
	
	m_AxialViewerCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_AxialViewerCommand->SetCallbackFunction(this, &IGMTracking::ProcessAxialViewInteraction);
	m_AxialViewer.AddObserver(ISIS::ClickedPointEvent(), m_AxialViewerCommand);
	
	m_CoronalViewerCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_CoronalViewerCommand->SetCallbackFunction(this, &IGMTracking::ProcessCoronalViewInteraction);
	m_CoronalViewer.AddObserver(ISIS::ClickedPointEvent(), m_CoronalViewerCommand);
	
	m_SaggitalViewerCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_SaggitalViewerCommand->SetCallbackFunction(this, &IGMTracking::ProcessSaggitalViewInteraction);
	m_SaggitalViewer.AddObserver(ISIS::ClickedPointEvent(), m_SaggitalViewerCommand);
	
	m_DicomReaderCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_DicomReaderCommand->SetCallbackFunction(this, &IGMTracking::ProcessDicomReaderInteraction);
	m_DicomVolumeReader.AddObserver( m_DicomReaderCommand );

	m_ProgressCommand = itk::SimpleMemberCommand<IGMTracking>::New();
	m_ProgressCommand->SetCallbackFunction(this, IGMTracking::ProcessDicomReading);
	m_DicomVolumeReader.m_Reader->AddObserver(itk::ProgressEvent(), m_ProgressCommand);

	m_Overlay = false;
	m_ShowVolume = false; //false;//
	m_EntryPointSelected = false; 
	m_TargetPointSelected = false;
//	m_RefRecorded = false;
	m_ForceRecord = false;

	this->SetNeedleLength( this->GetNeedleLength() );
	this->SetNeedleRadius( this->GetNeedleRadius() );
}




IGMTracking::~IGMTracking()
{
	if( m_ShiftScaleImageFilter )
	{
		m_ShiftScaleImageFilter->Delete();
	}
}



void IGMTracking::Show()
{
	IGMTrackingGUI::Show();

	mainWindow->position((Fl::w() - mainWindow->w()) / 2, (Fl::h() - mainWindow->h()) * 2 / 3);

	AxialView->show();
	CoronalView->show();
	SaggitalView->show();
	
	VolumeView->show();

	m_AxialViewer.SetInteractor( AxialView );
	m_CoronalViewer.SetInteractor( CoronalView );
	m_SaggitalViewer.SetInteractor( SaggitalView );

	m_MotionViewer.SetInteractor( VolumeView );

	AxialView->Initialize();
	CoronalView->Initialize();
	SaggitalView->Initialize();

	VolumeView->Initialize();

	m_MotionViewer.SetupCamera();

/*	if (m_ShowVolume)
	{
		VolumeView->show();
		m_VolumeViewer.SetInteractor( VolumeView );
		VolumeView->Initialize();
	}	*/
}



void IGMTracking::Hide()
{
	IGMTrackingGUI::Hide();
}



void IGMTracking::Quit()
{
//	for (int i = -80; i < 20; i++)
//	{
//		Sleep(5);
//		m_MotionViewer.UpdateMotion(i);
//	}

//	OnMotionTracking();

//	StopTracking();

	
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
	
	m_DicomVolumeReader.SetDirectory( directoryname );
	
	// Attempt to read
	m_DicomVolumeReader.CollectSeriesAndSelectOne();

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
	
	m_AxialViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );
	m_CoronalViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );
	m_SaggitalViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );
	
//	if (m_ShowVolume)
//		m_VolumeViewer.SetInput(m_ShiftScaleImageFilter->GetOutput());

	
	const unsigned int numberOfZslices = m_LoadedVolume->GetBufferedRegion().GetSize()[2];
	const unsigned int numberOfYslices = m_LoadedVolume->GetBufferedRegion().GetSize()[1];
	const unsigned int numberOfXslices = m_LoadedVolume->GetBufferedRegion().GetSize()[0];
	
	AxialViewSlider->bounds( 0.0, numberOfZslices-1 );
	AxialViewSlider->value( numberOfZslices / 2 );
	this->SelectAxialSlice( numberOfZslices / 2 );
	
	CoronalViewSlider->bounds( 0.0, numberOfYslices-1 );
	CoronalViewSlider->value( numberOfYslices / 2 );
	this->SelectCoronalSlice( numberOfYslices / 2 );
	
	SaggitalViewSlider->bounds( 0.0, numberOfXslices-1 );
	SaggitalViewSlider->value( numberOfXslices / 2 );
	this->SelectSaggitalSlice( numberOfXslices / 2 );
	
	this->DisplaySelectedPosition( false );

	this->DisplayTargetPosition( false );

	this->DisplayEntryPosition( false );

	this->DisplayToolPosition( false );

	m_Overlay = false;
	m_EntryPointSelected = false; 
	m_TargetPointSelected = false;

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();

//	if (m_ShowVolume)
//	{
//		m_VolumeViewer.Render();
//	}
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



void IGMTracking::SelectSaggitalSlice( int slice )
{
  m_SaggitalViewer.SelectSlice( slice );
  SaggitalView->redraw();
  Fl::check();
}


void IGMTracking::ProcessDicomReaderInteraction( void )
{
	this->PrintMessage( "Processing Dicom Reader Interaction " );
	bool volumeIsLoaded = m_DicomVolumeReader.IsVolumeLoaded();
	
	if( volumeIsLoaded )
    {
		m_LoadedVolume = m_DicomVolumeReader.GetOutput();
		this->LoadPostProcessing();
    }
}


void IGMTracking::DisplayEntryPosition( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateEntryPosition();
		m_CoronalViewer.ActivateEntryPosition();
		m_SaggitalViewer.ActivateEntryPosition();
	}
	else
	{
		m_AxialViewer.DeActivateEntryPosition();
		m_CoronalViewer.DeActivateEntryPosition();
		m_SaggitalViewer.DeActivateEntryPosition();
	}
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}
	


void IGMTracking::DisplaySelectedPosition( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateSelectedPosition();
		m_CoronalViewer.ActivateSelectedPosition();
		m_SaggitalViewer.ActivateSelectedPosition();
	}
	else
	{
		m_AxialViewer.DeActivateSelectedPosition();
		m_CoronalViewer.DeActivateSelectedPosition();
		m_SaggitalViewer.DeActivateSelectedPosition();
	}
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}


	
void IGMTracking::DisplayTargetPosition( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateTargetPosition();
		m_CoronalViewer.ActivateTargetPosition();
		m_SaggitalViewer.ActivateTargetPosition();
	}
	else
	{
		m_AxialViewer.DeActivateTargetPosition();
		m_CoronalViewer.DeActivateTargetPosition();
		m_SaggitalViewer.DeActivateTargetPosition();
	}

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}

void IGMTracking::DisplayEntryToTargetPath( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateEntryToTargetPathMarker();
		m_CoronalViewer.ActivateEntryToTargetPathMarker();
		m_SaggitalViewer.ActivateEntryToTargetPathMarker();
	}
	else
	{
		m_AxialViewer.DeActivateEntryToTargetPathMarker();
		m_CoronalViewer.DeActivateEntryToTargetPathMarker();
		m_SaggitalViewer.DeActivateEntryToTargetPathMarker();
	}

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}


void IGMTracking::DisplayToolPosition( const bool show)
{
	if (show)
	{
		m_AxialViewer.ActivateToolPosition();
		m_CoronalViewer.ActivateToolPosition();
		m_SaggitalViewer.ActivateToolPosition();
	}
	else
	{
		m_AxialViewer.DeActivateToolPosition();
		m_CoronalViewer.DeActivateToolPosition();
		m_SaggitalViewer.DeActivateToolPosition();
	}

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}
	

void IGMTracking::ProcessAxialViewInteraction( void )
{
	m_AxialViewer.GetSelectPoint( m_ClickedPoint );
	this->SyncAllViews( m_ClickedPoint );
	m_AxialViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_CoronalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_SaggitalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );	
	this->DisplaySelectedPosition( true );
}


void IGMTracking::ProcessCoronalViewInteraction( void )
{
	m_CoronalViewer.GetSelectPoint( m_ClickedPoint );
	this->SyncAllViews( m_ClickedPoint );
	m_AxialViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_CoronalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_SaggitalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );	
	this->DisplaySelectedPosition( true );
}


void IGMTracking::ProcessSaggitalViewInteraction( void )
{
	m_SaggitalViewer.GetSelectPoint( m_ClickedPoint );
	this->SyncAllViews( m_ClickedPoint );
	m_AxialViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_CoronalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );
	m_SaggitalViewer.SelectPoint( m_ClickedPoint[0], m_ClickedPoint[1], m_ClickedPoint[2] );	
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
	
	//sprintf(m_StringBuffer, "Clicked Point Index: %d x %d x %d", index[2], index[1], index[0]);
	//this->PrintMessage(m_StringBuffer);
	//printf(m_StringBuffer);
	
	
	// If the point is outside the volume, do not change slice selection
	if( m_LoadedVolume->GetLargestPossibleRegion().IsInside( index ) )
    {
		AxialViewSlider->value(    index[2]  );
		CoronalViewSlider->value(  index[1]  );
		SaggitalViewSlider->value( index[0]  );
		
		this->SelectAxialSlice(    index[2] );
		this->SelectCoronalSlice(  index[1] );
		this->SelectSaggitalSlice( index[0] );
    }
	else
	{
		this->PrintMessage("SyncAllViews::Clicked point is outside the volume");
	}
	
	// Sync the selected point in all the views even if it is outside the image
	//m_AxialViewer.SeedPoint( aboutPoint[0], aboutPoint[1], aboutPoint[2] );
	//m_CoronalViewer.SeedPoint( aboutPoint[0], aboutPoint[1], aboutPoint[2] );
	//m_SaggitalViewer.SeedPoint( aboutPoint[0], aboutPoint[1], aboutPoint[2] );	
}


void IGMTracking::OnTargetPoint( void )
{
	IGMTrackingBase::SetTargetPoint( m_ClickedPoint );
	sprintf(m_StringBuffer, "Target Point = [%4.3f %4.3f %4.3f]", 
		m_TargetPoint[0],  m_TargetPoint[1],  m_TargetPoint[2] );
	this->PrintMessage(m_StringBuffer);

	m_AxialViewer.m_TargetPositionMarker.SetCenter( m_ClickedPoint );
	m_CoronalViewer.m_TargetPositionMarker.SetCenter( m_ClickedPoint );
	m_SaggitalViewer.m_TargetPositionMarker.SetCenter( m_ClickedPoint );

	this->DisplaySelectedPosition( false );
	this->DisplayTargetPosition( true );

	m_TargetPointSelected = true;
	if (m_EntryPointSelected)
	{
		this->DisplayEntryToTargetPath( true );
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
	m_SaggitalViewer.m_EntryPositionMarker.SetCenter( m_ClickedPoint );

	this->DisplaySelectedPosition( false );
	this->DisplayEntryPosition( true );

	m_EntryPointSelected = true; 
	if (m_TargetPointSelected)
	{
		this->DisplayEntryToTargetPath( true );
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
		fl_alert("Initialization of Aurora Failed...");
	}
	else
	{
		this->PrintMessage("Initialization Aurora Succeeded...");
		fl_alert("Initialization of Aurora Succeeded...");
	}
}


void IGMTracking::ActivateTools( void )
{
	if (!m_AuroraTracker.ActivateTools())
	{
		this->PrintMessage("Activation of Aurora Tools Failed...");
		fl_alert("Activation of Aurora Tools Failed...");
	}
	else
	{
		this->PrintMessage("Activation of Aurora Tools Succeeded...");
		fl_alert("Activation of Aurora Tools Succeeded...");
	}
}


void IGMTracking::StartTracking( void )
{	
	m_AuroraTracker.StartTracking();
	char  toolID[10];
	sprintf(toolID, "%02d", 1 );
	m_ToolHandle = m_AuroraTracker.GetMyToolHandle( toolID );
	if (m_ToolHandle<0)
	{
		fl_alert("No tool on Port 0 detected ...");
	}
	else
	{
		fl_alert("Tool on Port 0 recognised ...");
	}
	if (m_UseReferenceNeedle)
	{
		sprintf(toolID, "%02d", 2 );
		m_ReferenceToolHandle = m_AuroraTracker.GetMyToolHandle( toolID );
		if (m_ReferenceToolHandle<0)
		{
			fl_alert("No reference tool on Port 1 detected ...");
		}
		else
		{
			fl_alert("Reference Tool on Port 1 recognised ...");
		}
	}
}

void IGMTracking::StopTracking( void )
{
	m_AuroraTracker.StopTracking();
}

void IGMTracking::PrintToolPosition( const int toolHandle )
{
	if ( m_AuroraTracker.m_HandleInformation[toolHandle].Xfrms.ulFlags == TRANSFORM_VALID )
	{
		//sprintf(m_StringBuffer, "Needle Tip Position: X = %.2f, Y = %.2f, Z = %.2f", 
		//			m_NeedlePosition[0], m_NeedlePosition[1], m_NeedlePosition[2]); 
		//this->PrintMessage(m_StringBuffer);
		
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

// The function ThreadFunction is not being used anymore.	
//void ThreadFunction(void* data)
//{
//	IGMTracking* igmTracking = (IGMTracking*) data;
//	while(igmTracking->m_Overlay)
//	{
//		Fl::lock();
//		igmTracking->OnToolPosition();
//		Fl::unlock();
//		Fl::awake(data);
//		Sleep(5000);
//	}
//	igmTracking->DisplaySelectedPosition( true );
//	_endthread();
//}

bool	continueTracking;

void CheckKeyPress(void *dummy)
{
	_getch();
//	printf("Key Press detected ****************\n");
	continueTracking = false;
//	_endthread();
}


bool IGMTracking::OnRegister( void )
{
	

	return this->RegisterImageWithTrackerAndComputeRMSError();
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
	bool firstpos = true;
	double HomePos[3], d[3], dis;
	double toolPositionInImageSpace[3], anotherPositionInImageSpace[3];

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
			
			double anotherPosition[3];
			
			m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ToolHandle, m_NeedleEndOffset, anotherPosition);
			
			if (m_UseReferenceNeedle)
			{
				double refToolNewPosition[3], offset[3];
				m_AuroraTracker.GetOffsetCorrectedToolPosition( m_ReferenceToolHandle, 
					m_ReferenceNeedleTipOffset, refToolNewPosition);
				
				for(int i=0; i<3; i++)
				{
					offset[i] = refToolNewPosition[i] - m_ReferenceNeedlePosition[i];
					m_NeedlePosition[i] -= offset[i];
					anotherPosition[i] -= offset[i];
				}

				for (int i = 0; i < 3; i++)
				{
					d[i] = HomePos[i] - refToolNewPosition[i];
				}
				
				dis = sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
				m_MotionViewer.UpdateMotion(dis);
			}
			
//			double toolPositionInImageSpace[3], anotherPositionInImageSpace[3];
			
			m_FiducialRegistration.FromSourceSpaceToDestinationSpace(m_NeedlePosition,toolPositionInImageSpace);
			m_FiducialRegistration.FromSourceSpaceToDestinationSpace(anotherPosition,anotherPositionInImageSpace);
			
			m_AxialViewer.m_ToolPositionMarker.SetTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
			m_AxialViewer.MakeToolPositionMarkerVisible();
			m_CoronalViewer.m_ToolPositionMarker.SetTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
			m_CoronalViewer.MakeToolPositionMarkerVisible();
			m_SaggitalViewer.m_ToolPositionMarker.SetTipAndDirection(toolPositionInImageSpace,anotherPositionInImageSpace);
			m_SaggitalViewer.MakeToolPositionMarkerVisible();

			sprintf(m_StringBuffer, "%4.3f   %4.3f   %4.3f\n", toolPositionInImageSpace[0], toolPositionInImageSpace[1], toolPositionInImageSpace[2]);
			if (fp != NULL) fprintf(fp, m_StringBuffer);
			printf( m_StringBuffer );

			this->SyncAllViews( toolPositionInImageSpace );
			
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
{
	return m_ShiftScaleImageFilter->GetScale();
}
	
void IGMTracking::SetImageScale( double val )
{
	m_ShiftScaleImageFilter->SetScale( val );
	m_ShiftScaleImageFilter->UpdateWholeExtent();
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}

	
double IGMTracking::GetImageShift( void )
{
	return m_ShiftScaleImageFilter->GetShift();
}
	
void IGMTracking::SetImageShift( double val )
{
	m_ShiftScaleImageFilter->SetShift( val );
	m_ShiftScaleImageFilter->UpdateWholeExtent();
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}

void IGMTracking::SetNeedleLength( const double val )
{
	IGMTrackingBase::SetNeedleLength( val );
	m_AxialViewer.m_ToolPositionMarker.SetLength(val);
	m_CoronalViewer.m_ToolPositionMarker.SetLength(val);
	m_SaggitalViewer.m_ToolPositionMarker.SetLength(val);
}
	
void IGMTracking::SetNeedleRadius( const double val )
{
	IGMTrackingBase::SetNeedleRadius( val );
	m_AxialViewer.m_ToolPositionMarker.SetRadius(val);
	m_CoronalViewer.m_ToolPositionMarker.SetRadius(val);
	m_SaggitalViewer.m_ToolPositionMarker.SetRadius(val);
}
	

void IGMTracking::PrintDebugInfo( void )
{
	double pos1[3], pos2[3];
	printf("Debug Information:\n");
	m_SaggitalViewer.m_TargetPositionMarker.GetCenter( pos1 );
	printf("Target: %4.3f   %4.3f   %4.3f\n", pos1[0], pos1[1], pos1[2]);
	m_SaggitalViewer.m_EntryPositionMarker.GetCenter( pos1 );
	printf("Entry: %4.3f   %4.3f   %4.3f\n", pos1[0], pos1[1], pos1[2]);
	m_SaggitalViewer.m_EntryToTargetPathMarker.m_LineSource->GetPoint1( pos1 );
	m_SaggitalViewer.m_EntryToTargetPathMarker.m_LineSource->GetPoint2( pos2 );
	printf("Line Point 01: %4.3f   %4.3f   %4.3f\n", pos1[0], pos1[1], pos1[2]);
	printf("Line Point 02: %4.3f   %4.3f   %4.3f\n", pos2[0], pos2[1], pos2[2]);
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
	double f = m_DicomVolumeReader.m_Reader->GetProgress();
	printf("%.3f\n", f);

	m_ProgressBar->value(f);
	m_ProgressBar->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	m_ProgressDial->value(f);
	m_ProgressDial->color(fl_rgb_color((uchar)(255 - f * 255), 0, (uchar)(f * 255)));

	Fl::check();
}