


#include "LiverTumorSegmentation.h"

#include "FL/Fl_File_Chooser.H"

#include "vtkImageShiftScale.h"

#include "ClickedPointEvent.h"

#include "vtkImageBlend.h"

LiverTumorSegmentation::LiverTumorSegmentation()
{
	m_AxialViewer.SetOrientation(    ISIS::ImageSliceViewer::Axial    );
	m_CoronalViewer.SetOrientation(  ISIS::ImageSliceViewer::Coronal  );
	m_SaggitalViewer.SetOrientation( ISIS::ImageSliceViewer::Saggital );
	
	m_ShiftScaleImageFilter = vtkImageShiftScale::New();
	
	m_ShiftScaleImageFilter->SetInput(  m_ITK2VTKAdaptor->GetOutput() );
	m_ShiftScaleImageFilter->SetOutputScalarTypeToUnsignedChar();
	m_ShiftScaleImageFilter->ClampOverflowOn();
	
	m_AxialViewerCommand = itk::SimpleMemberCommand<LiverTumorSegmentation>::New();
	m_AxialViewerCommand->SetCallbackFunction(this, &LiverTumorSegmentation::ProcessAxialViewInteraction);
	m_AxialViewer.AddObserver(ISIS::ClickedPointEvent(), m_AxialViewerCommand);
	
	m_CoronalViewerCommand = itk::SimpleMemberCommand<LiverTumorSegmentation>::New();
	m_CoronalViewerCommand->SetCallbackFunction(this, &LiverTumorSegmentation::ProcessCoronalViewInteraction);
	m_CoronalViewer.AddObserver(ISIS::ClickedPointEvent(), m_CoronalViewerCommand);
	
	m_SaggitalViewerCommand = itk::SimpleMemberCommand<LiverTumorSegmentation>::New();
	m_SaggitalViewerCommand->SetCallbackFunction(this, &LiverTumorSegmentation::ProcessSaggitalViewInteraction);
	m_SaggitalViewer.AddObserver(ISIS::ClickedPointEvent(), m_SaggitalViewerCommand);
	
	m_DicomReaderCommand = itk::SimpleMemberCommand<LiverTumorSegmentation>::New();
	m_DicomReaderCommand->SetCallbackFunction(this, &LiverTumorSegmentation::ProcessDicomReaderInteraction);
	m_DicomVolumeReader.AddObserver( m_DicomReaderCommand );
	
	m_ProgressSlider->Observe( m_LiverTumorSegmentationModule.GetNotifier() );

	m_BackgroundVolume = INPUT_VOLUME;
	m_OverlayVolume = SEGMENTED_VOLUME;
	m_OverlayedVolumeOpacity = 0.5;

	this->SetBackgroundVolumeType( m_BackgroundVolume );
	this->SetOverlayVolumeType( m_OverlayVolume );

    m_vtkImageBlender = vtkImageBlend::New();
    m_vtkImageBlender->SetInput( 0, m_ITK2VTKAdaptor->GetOutput() );
    m_vtkImageBlender->SetInput( 1, m_OverlayVolumeITK2VTKAdaptor->GetOutput() );
	m_vtkImageBlender->SetOpacity(0, 1.0 - m_OverlayedVolumeOpacity );
	m_vtkImageBlender->SetOpacity(1, m_OverlayedVolumeOpacity );

    m_ShiftScaleImageFilter->SetInput( m_vtkImageBlender->GetOutput() );

	m_ShowVolumeView = true;
}


LiverTumorSegmentation::~LiverTumorSegmentation()
{
	if( m_ShiftScaleImageFilter )
	{
		m_ShiftScaleImageFilter->Delete();
	}
}


void LiverTumorSegmentation::Show()
{
	LiverTumorSegmentationGUI::Show();

	axialView->show();
	coronalView->show();
	saggitalView->show();
	
	m_AxialViewer.SetInteractor( axialView );
	m_CoronalViewer.SetInteractor( coronalView );
	m_SaggitalViewer.SetInteractor( saggitalView );
	
	axialView->Initialize();
	coronalView->Initialize();
	saggitalView->Initialize();

	if (m_ShowVolumeView)
	{
		volumeView->show();
		m_VolumeViewer.SetInteractor( volumeView );
		volumeView->Initialize();
	}	
}



void LiverTumorSegmentation::Hide()
{
	LiverTumorSegmentationGUI::Hide();
}



void LiverTumorSegmentation::Quit()
{
	this->Hide();
}


void LiverTumorSegmentation::Load( void )
{
	const char * filename = fl_file_chooser("Volume filename","*.*","");
	
	if (LiverTumorSegmentationBase::Load( filename ) )
    {
		this->LoadPostProcessing();
		sprintf( m_MessageString, "File %s has been loaded successfully.", filename );
		m_MessageBar->label( m_MessageString );
	}
	else
	{
		sprintf( m_MessageString, "File %s loading unsuccessful.", filename );
		m_MessageBar->label( m_MessageString );
	}
}



void LiverTumorSegmentation::LoadDICOM()
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



void LiverTumorSegmentation::LoadPostProcessing()
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
	
	VisualizationVolumeType::IndexType index; 
	itk::Point< float, 3 >	point;		
	index[0] = numberOfXslices / 2;
	index[1] = numberOfYslices / 2;
	index[2] = numberOfZslices / 2;
	m_LoadedVolume->TransformIndexToPhysicalPoint( index, point );
	this->SetSeedPoint( point[0], point[1], point[2] );

	if (m_ShowVolumeView)
	{
		m_VolumeViewer.SetInput(m_ShiftScaleImageFilter->GetOutput());
		m_VolumeViewer.Render();
	}

}


void LiverTumorSegmentation::SelectAxialSlice( int slice )
{
	m_AxialViewer.SelectSlice( slice );
	axialView->redraw();
	Fl::check();
}


void LiverTumorSegmentation::SelectCoronalSlice( int slice )
{
	m_CoronalViewer.SelectSlice( slice );
	coronalView->redraw();
	Fl::check();
}


void LiverTumorSegmentation::SelectSaggitalSlice( int slice )
{
	m_SaggitalViewer.SelectSlice( slice );
	saggitalView->redraw();
	Fl::check();
}


void LiverTumorSegmentation::ProcessDicomReaderInteraction( void )
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
LiverTumorSegmentation::SetSeedPoint( float x, float y, float z )
{
	if (!m_LoadedVolume) return ;
	LiverTumorSegmentationBase::SetSeedPoint( x, y, z );
	this->SyncAllViews();
}

void LiverTumorSegmentation::ProcessAxialViewInteraction( void )
{
	m_AxialViewer.GetSelectPoint( m_SeedPoint );
	this->SyncAllViews();
}


void LiverTumorSegmentation::ProcessCoronalViewInteraction( void )
{
	m_CoronalViewer.GetSelectPoint( m_SeedPoint );
	this->SyncAllViews();
}


void LiverTumorSegmentation::ProcessSaggitalViewInteraction( void )
{
	m_SaggitalViewer.GetSelectPoint( m_SeedPoint );
	this->SyncAllViews();
}


void LiverTumorSegmentation::SyncAllViews(void)
{
	sprintf( m_MessageString, "%5.2f", m_SeedPoint[0] );
	m_SeedX->value( m_MessageString );
	sprintf( m_MessageString, "%5.2f", m_SeedPoint[1] );
	m_SeedY->value( m_MessageString );
	sprintf( m_MessageString, "%5.2f", m_SeedPoint[2] );
	m_SeedZ->value( m_MessageString );

	itk::Point< float, 3 > point;
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
		
		m_SeedValue = m_LoadedVolume->GetPixel( index );
		
		m_SeedIndex[0] = index[0];
		m_SeedIndex[1] = index[1];
		m_SeedIndex[2] = index[2];
    }
	// Sync the selected point in all the views even if it is outside the image
	sprintf( m_MessageString, "Clicked Point:Indices(%d,%d,%d),Position(%4.3f,%4.3f,%4.3f),Value(%4.3f)", 
		index[0],index[1],index[2],m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2], m_SeedValue );
	printf( m_MessageString );
	printf("\n");

	m_MessageBar->label( m_MessageString );

	m_AxialViewer.SelectPoint(    m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
	m_CoronalViewer.SelectPoint(  m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
	m_SaggitalViewer.SelectPoint( m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
	
}


void LiverTumorSegmentation::OnSegmentation( void )
{
	printf("Doing Segmentation ...\n");
	if (LiverTumorSegmentationBase::DoSegmentation())
	{
//		if (m_OverlayVolume!=SEGMENTED_VOLUME)
		{ 
			this->SetOverlayVolumeType( SEGMENTED_VOLUME );
		}

		try
		{
			m_ShiftScaleImageFilter->UpdateWholeExtent();
		}
		catch( itk::ExceptionObject & exp )
		{
			fl_message( exp.GetDescription() );
			m_MessageBar->label( "Segmentation Unsuccessful." );
			return;
		}

		m_AxialViewer.Render();
		m_CoronalViewer.Render();
		m_SaggitalViewer.Render();

		printf( "Elapsed iterations = %d\n", m_LiverTumorSegmentationModule.GetElapsedIterations());
		
		m_MessageBar->label( "Segmentation completed." );
	}
	else
	{
		fl_alert("Segmentation unsuccessful.");
		m_MessageBar->label( "Segmentation Unsuccessful." );
	}
}



void LiverTumorSegmentation::OnThreshold( const float lower, const float upper )
{
	if (LiverTumorSegmentationBase::DoThreshold( lower, upper ))
	{
//		if (m_OverlayVolume!=THRESHOLDED_VOLUME)
		{
			this->SetOverlayVolumeType( THRESHOLDED_VOLUME );
		}

		try
		{
			m_ShiftScaleImageFilter->UpdateWholeExtent();
		}
		catch( itk::ExceptionObject & exp )
		{
			fl_message( exp.GetDescription() );
			return;
		}

		m_AxialViewer.Render();
		m_CoronalViewer.Render();
		m_SaggitalViewer.Render();
	}
	else
	{
		fl_alert("Thresholding unsuccessful.");
	}
}



float LiverTumorSegmentation::GetImageScale( void )
{
	return m_ShiftScaleImageFilter->GetScale();
}



void LiverTumorSegmentation::SetImageScale( float val )
{
	m_ShiftScaleImageFilter->SetScale( val );
	m_ShiftScaleImageFilter->UpdateWholeExtent();
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}



float LiverTumorSegmentation::GetImageShift( void )
{
	return m_ShiftScaleImageFilter->GetShift();
}



void LiverTumorSegmentation::SetImageShift( float val )
{
	m_ShiftScaleImageFilter->SetShift( val );
	m_ShiftScaleImageFilter->UpdateWholeExtent();
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}


float LiverTumorSegmentation::GetOverlayedVolumeOpacity( void )
{
	return m_OverlayedVolumeOpacity;
}


bool LiverTumorSegmentation::SetOverlayedVolumeOpacity( const float value )
{
	if ((value>=0.0f) && (value<=1.0f))
	{
		m_OverlayedVolumeOpacity = value;

		m_vtkImageBlender->SetOpacity(0, 1.0 - m_OverlayedVolumeOpacity );
		m_vtkImageBlender->SetOpacity(1, m_OverlayedVolumeOpacity );

		return true;
	}
	return false;
}

void LiverTumorSegmentation::OnImageControl( void )
{
	m_BackgroundVolumeList->clear();
	m_OverlayVolumeList->clear();

	for(int i=0; i<3; i++)
	{
		m_BackgroundVolumeList->add( Volumes[i] );
		m_OverlayVolumeList->add( Volumes[i] );
	}
	m_BackgroundVolumeList->value( (int) m_BackgroundVolume );
	m_OverlayVolumeList->value( (int) m_OverlayVolume );
	m_OverlayOpacity->value( m_OverlayedVolumeOpacity );	
}

void LiverTumorSegmentation::OnImageControlOk( void )
{
//	if (m_BackgroundVolume!=m_BackgroundVolumeList->value())
	{
		this->SetBackgroundVolumeType((DisplayVolumeType)m_BackgroundVolumeList->value());
	}
//	if (m_OverlayVolume!=m_OverlayVolumeList->value())
	{
		this->SetOverlayVolumeType((DisplayVolumeType)m_OverlayVolumeList->value());
	}
//	if ( m_OverlayedVolumeOpacity!=m_OverlayOpacity->value())
	{
		this->SetOverlayedVolumeOpacity( m_OverlayOpacity->value() );
	}
	m_ShiftScaleImageFilter->UpdateWholeExtent();
	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}


DisplayVolumeType LiverTumorSegmentation::GetBackgroundVolumeType( void )
{
	return m_BackgroundVolume;
}


void LiverTumorSegmentation::SetBackgroundVolumeType( DisplayVolumeType type )
{
	m_BackgroundVolume = type;
	switch(type)
	{
	case INPUT_VOLUME: 
		m_RescaleIntensity->SetInput( m_LoadedVolume );
		break;
	case SEGMENTED_VOLUME:
		m_RescaleIntensity->SetInput( m_SegmentedVolume );
		break;
	case THRESHOLDED_VOLUME:
		m_RescaleIntensity->SetInput( m_ThresholdedVolume );
		break;
	}
}


DisplayVolumeType LiverTumorSegmentation::GetOverlayVolumeType( void )
{
	return m_OverlayVolume;
}


void LiverTumorSegmentation::SetOverlayVolumeType( DisplayVolumeType type )
{
	m_OverlayVolume = type;
	switch(type)
	{
	case INPUT_VOLUME: 
		m_OverlayVolumeRescaleIntensity->SetInput( m_LoadedVolume );
		break;
	case SEGMENTED_VOLUME:
		m_OverlayVolumeRescaleIntensity->SetInput( m_SegmentedVolume );
		break;
	case THRESHOLDED_VOLUME:
		m_OverlayVolumeRescaleIntensity->SetInput( m_ThresholdedVolume );
		break;
	}
}


void LiverTumorSegmentation::LoadSegmentedVolume( void )
{
	const char *filename = fl_file_chooser("Segmented Volume Filename","*.*","");

	temp_VolumeReader->SetFileName( filename );
	temp_VolumeReader->Update();
	m_SegmentedVolume = temp_VolumeReader->GetOutput();
		
	this->SetBackgroundVolumeType( INPUT_VOLUME );
	this->SetOverlayVolumeType( SEGMENTED_VOLUME );

	m_ShiftScaleImageFilter->UpdateWholeExtent();

	m_AxialViewer.Render();
	m_CoronalViewer.Render();
	m_SaggitalViewer.Render();
}


void LiverTumorSegmentation::DoDebug( void )
{
//	m_SeedIndex[0] = 56;
//	m_SeedIndex[1] = 69;
//	m_SeedIndex[2] = 91;
//	this->OnSegmentation();
}


