#include "IGMTrackingBase.h"


IGMTrackingBase::IGMTrackingBase()
{
  // 1
  m_VolumeReader      = VolumeReaderType::New();

	m_RescaleIntensity  = RescaleIntensityFilterType::New();
  m_RescaleIntensity->SetOutputMaximum( itk::NumericTraits< VisualizationPixelType >::max() );
	m_RescaleIntensity->SetOutputMinimum( itk::NumericTraits< VisualizationPixelType >::min() );
  
  m_ITK2VTKAdaptor    = ITK2VTKAdaptorFilterType::New();
	m_ITK2VTKAdaptor->SetInput( m_RescaleIntensity->GetOutput() );
  
  // 2
  m_FusionVolumeReader      = VolumeReaderType::New();

  m_Transform = TransformType::New();
  m_LInterpolate = LInterpolateType::New();
  m_NNInterpolate = NNInterpolateType::New();

  m_ResampleImageFusionFilter = ResampleImageFilterType::New();
  m_ResampleImageFusionFilter->SetTransform(m_Transform);
  m_ResampleImageFusionFilter->SetInterpolator(m_LInterpolate);
  m_ResampleImageFusionFilter->SetDefaultPixelValue(0);
	
  m_FusionRescaleIntensity  = RescaleIntensityFilterType::New();
  m_FusionRescaleIntensity->SetInput(m_ResampleImageFusionFilter->GetOutput());
  m_FusionRescaleIntensity->SetOutputMaximum( itk::NumericTraits< VisualizationPixelType >::max() );
	m_FusionRescaleIntensity->SetOutputMinimum( itk::NumericTraits< VisualizationPixelType >::min() );
	
  m_FusionITK2VTKAdaptor    = ITK2VTKAdaptorFilterType::New();
	m_FusionITK2VTKAdaptor->SetInput( m_FusionRescaleIntensity->GetOutput() );
	
  // 3
  m_ResampleImagePreFilter = ResampleImageFilterType::New();
  m_ResampleImagePreFilter->SetTransform(m_Transform);
  m_ResampleImagePreFilter->SetInterpolator(m_LInterpolate);
  m_ResampleImagePreFilter->SetDefaultPixelValue(0);
  
  m_ConfidenceConnectedFilter = ConfidenceConnectedFilterType::New();
  m_ConfidenceConnectedFilter->SetInput(m_ResampleImagePreFilter->GetOutput());
  m_ConfidenceConnectedFilter->SetMultiplier(2.5);
  m_ConfidenceConnectedFilter->SetNumberOfIterations(5);
  m_ConfidenceConnectedFilter->SetReplaceValue(255);
  m_ConfidenceConnectedFilter->SetInitialNeighborhoodRadius(1);

  m_ConnectedThresholdFilter = ConnectedThresholdFilterType::New();
  m_ConnectedThresholdFilter->SetInput(m_ResampleImagePreFilter->GetOutput());
  m_ConnectedThresholdFilter->SetLower(80);
  m_ConnectedThresholdFilter->SetUpper(100);
  m_ConnectedThresholdFilter->SetReplaceValue(255);

  VolumeType::SizeType radius;
  radius[0] = 1;
  radius[1] = 1;

  m_NeighborhoodConnectedFilter = NeighborhoodConnectedFilterType::New();
  m_NeighborhoodConnectedFilter->SetInput(m_ResampleImagePreFilter->GetOutput());
  m_NeighborhoodConnectedFilter->SetLower(80);
  m_NeighborhoodConnectedFilter->SetUpper(100);
  m_NeighborhoodConnectedFilter->SetRadius(radius);
  m_NeighborhoodConnectedFilter->SetReplaceValue(255);

  m_IsolatedConnectedFilter = IsolatedConnectedFilterType::New();
  m_IsolatedConnectedFilter->SetInput(m_ResampleImagePreFilter->GetOutput());
  m_IsolatedConnectedFilter->SetLower(0);
  m_IsolatedConnectedFilter->SetReplaceValue(255);

  m_ResampleImagePostFilter = ResampleImageFilterType::New();
//  m_ResampleImagePostFilter->SetInput(m_ConfidenceConnectedFilter->GetOutput());
  m_ResampleImagePostFilter->SetTransform(m_Transform);
  m_ResampleImagePostFilter->SetInterpolator(m_LInterpolate);
  m_ResampleImagePostFilter->SetDefaultPixelValue(0);
/*  
  m_ImageResample = vtkImageResample::New();

  m_ImageReslice = vtkImageReslice::New();

  m_ImageMap = vtkImageMapToRGBA::New();

  m_LUT = vtkLookupTable::New();
*/

  // 4
//  m_ImageBlend = vtkImageBlend::New();
/*
  m_ImageBlend->SetInput(0, m_ITK2VTKAdaptor->GetOutput());
  m_ImageResample->SetInput(m_FusionITK2VTKAdaptor->GetOutput());
  m_ImageBlend->SetInput(1, m_ImageResample->GetOutput());
*/
//  m_ImageBlend->SetOpacity(0, 1.0);
//  m_ImageBlend->SetOpacity(1, 0.0);
  
  m_FusionOpacity = 0.5;
  m_Pipeline = 1;

	m_NeedleTipOffset[0] = 0.0f;
	m_NeedleTipOffset[1] = 0.0f;
	m_NeedleTipOffset[2] = 10.4f; 

	m_NeedleEndOffset[0] = 0.0f;
	m_NeedleEndOffset[1] = 0.0f;
	m_NeedleEndOffset[2] = 210.4f; 

	m_NeedleLength = 100.0f; 
	m_NeedleRadius = 1.5f;

	m_UseReferenceNeedle = false;
	m_ReferenceNeedleTipOffset[0] = 0.0f;
	m_ReferenceNeedleTipOffset[1] = 0.0f;
	m_ReferenceNeedleTipOffset[2] = 19.1f; 
}

IGMTrackingBase::~IGMTrackingBase()
{
/*  if (m_ImageBlend)
  {
    m_ImageBlend->Delete();
  }

  if (m_ImageResample)
  {
    m_ImageResample->Delete();
  }

  if (m_ImageReslice)
  {
    m_ImageReslice->Delete();
  }

  if (m_ImageMap)
  {
    m_ImageMap->Delete();
  }

  if (m_LUT)
  {
    m_LUT->Delete();
  }
*/ 
}


void IGMTrackingBase::SetTargetPoint( const double point[3] )
{
	for(int i=0; i<3; i++)
	{
		m_TargetPoint[i] = point[i];
	}
}


void IGMTrackingBase::SetTargetPoint( double x, double y, double z )
{
	m_TargetPoint[0] = x;
	m_TargetPoint[1] = y;
	m_TargetPoint[2] = z;
}


void IGMTrackingBase::SetEntryPoint( const double point[3] )
{
	for(int i=0; i<3; i++)
	{
		m_EntryPoint[i] = point[i];
	}
}


void IGMTrackingBase::SetEntryPoint( double x, double y, double z )
{
	m_EntryPoint[0] = x;
	m_EntryPoint[1] = y;
	m_EntryPoint[2] = z;
}


void IGMTrackingBase::SetNumberOfFiducials( unsigned int number )
{
	m_FiducialRegistration.DeleteFiducials();
	m_FiducialRegistration.SetNumberOfFiducials(number);
}


unsigned int IGMTrackingBase::GetNumberOfFiducials( void )
{
	return m_FiducialRegistration.GetNumberOfFiducials();
}


void IGMTrackingBase::SetImageFiducial( unsigned int index, double *position )
{
	//	std::cout << "Image Fiducial " << index << " set at ( " << position[0] << "," << position[1] << "," << position[2] << ")" << std::endl;
	m_FiducialRegistration.SetDestinationFiducial( index, position );
}



void IGMTrackingBase::SetTrackerFiducial( unsigned int index, double *position )
{
  if (index != -1)
  {
	  m_FiducialRegistration.SetSourceFiducial( index, position );
  }
}



void IGMTrackingBase::PrintMessage( char *msg )
{
	std::cout << msg << std::endl; 
}


bool IGMTrackingBase::RegisterImageWithTrackerAndComputeRMSError( void )
{
	if (!m_FiducialRegistration.AreDestinationFiducialsInitialized())
	{
		this->PrintMessage("Image Fiducials not initialized.");
		return false;
	}
	
	if (!m_FiducialRegistration.AreSourceFiducialsInitialized())
	{
		this->PrintMessage("Tracker Fiducials not initialized.");
		return false;
	}
	
	sprintf(m_StringBuffer, "RMS Error = %4.3f", m_FiducialRegistration.GetRMSError());
	this->PrintMessage( m_StringBuffer );
	return true;
}


void IGMTrackingBase::SetNeedleTipOffset( const double offsetX, const double offsetY, const double offsetZ )
{
	m_NeedleTipOffset[0] = offsetX;
	m_NeedleTipOffset[1] = offsetY;
	m_NeedleTipOffset[2] = offsetZ;
}


void IGMTrackingBase::GetNeedleTipOffset( double& offsetX, double& offsetY, double& offsetZ )
{
	offsetX = m_NeedleTipOffset[0];
	offsetY = m_NeedleTipOffset[1];
	offsetZ = m_NeedleTipOffset[2];
}


void IGMTrackingBase::SetNeedleEndOffset( const double offsetX, const double offsetY, const double offsetZ )
{
	m_NeedleEndOffset[0] = offsetX;
	m_NeedleEndOffset[1] = offsetY;
	m_NeedleEndOffset[2] = offsetZ;
}


void IGMTrackingBase::GetNeedleEndOffset( double& offsetX, double& offsetY, double& offsetZ )
{
	offsetX = m_NeedleEndOffset[0];
	offsetY = m_NeedleEndOffset[1];
	offsetZ = m_NeedleEndOffset[2];
}


void IGMTrackingBase::SetReferenceNeedleTipOffset( const double offsetX, const double offsetY, const double offsetZ )
{
	m_ReferenceNeedleTipOffset[0] = offsetX;
	m_ReferenceNeedleTipOffset[1] = offsetY;
	m_ReferenceNeedleTipOffset[2] = offsetZ;
}


void IGMTrackingBase::GetReferenceNeedleTipOffset( double& offsetX, double& offsetY, double& offsetZ )
{
	offsetX = m_ReferenceNeedleTipOffset[0];
	offsetY = m_ReferenceNeedleTipOffset[1];
	offsetZ = m_ReferenceNeedleTipOffset[2];
}

void IGMTrackingBase::SetUseReferenceNeedleOption( const bool option )
{
	m_UseReferenceNeedle = option;
}

bool IGMTrackingBase::GetUseReferenceNeedleOption( void )
{
	return m_UseReferenceNeedle;
}


double IGMTrackingBase::GetNeedleLength( void )
{
	return m_NeedleLength; 
}


	
void IGMTrackingBase::SetNeedleLength( const double val )
{
	m_NeedleLength = val;
}
	


double IGMTrackingBase::GetNeedleRadius( void )
{
	return 	m_NeedleRadius;
}


	
void IGMTrackingBase::SetNeedleRadius( const double val )
{
	m_NeedleRadius = val;
}
	
void IGMTrackingBase::OnUpdateLayout()
{

}

void IGMTrackingBase::OnUpdateProbeParameters()
{

}

void IGMTrackingBase::OnUpdateOpacity(double opa)
{
/*  m_ImageBlend->SetOpacity(0, 1.0 - opa);
  m_ImageBlend->SetOpacity(1, opa);

  m_FusionOpacity = opa;*/
}

void IGMTrackingBase::RenderSectionWindow()
{

}

void IGMTrackingBase::RenderAllWindow()
{

}

void IGMTrackingBase::SetInputData(int pipeline)
{

}

void IGMTrackingBase::OnUpdateSegParameters()
{

}
