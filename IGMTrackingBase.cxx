#include "IGMTrackingBase.h"


IGMTrackingBase::IGMTrackingBase()
{
	m_VolumeReader      = VolumeReaderType::New();
	m_RescaleIntensity  = RescaleIntensityFilterType::New();

	m_ITK2VTKAdaptor    = ITK2VTKAdaptorFilterType::New();
	m_ITK2VTKAdaptor->SetInput( m_RescaleIntensity->GetOutput() );
	
	m_RescaleIntensity->SetOutputMaximum( itk::NumericTraits< VisualizationPixelType >::max() );
	m_RescaleIntensity->SetOutputMinimum( itk::NumericTraits< VisualizationPixelType >::min() );

	m_NeedleTipOffset[0] = 0.0f;
	m_NeedleTipOffset[1] = 0.0f;
	m_NeedleTipOffset[2] = -10.4f; 

	m_NeedleEndOffset[0] = 0.0f;
	m_NeedleEndOffset[1] = 0.0f;
	m_NeedleEndOffset[2] = -210.4f; 

	m_NeedleLength = 100.0f; 
	m_NeedleRadius = 1.5f;

	m_UseReferenceNeedle = false;
	m_ReferenceNeedleTipOffset[0] = 0.0f;
	m_ReferenceNeedleTipOffset[1] = 0.0f;
	m_ReferenceNeedleTipOffset[2] = -19.1f; 
}




IGMTrackingBase::~IGMTrackingBase()
{
}


void IGMTrackingBase::SetTargetPoint( const float point[3] )
{
	for(int i=0; i<3; i++)
	{
		m_TargetPoint[i] = point[i];
	}
}


void IGMTrackingBase::SetTargetPoint( float x, float y, float z )
{
	m_TargetPoint[0] = x;
	m_TargetPoint[1] = y;
	m_TargetPoint[2] = z;
}


void IGMTrackingBase::SetEntryPoint( const float point[3] )
{
	for(int i=0; i<3; i++)
	{
		m_EntryPoint[i] = point[i];
	}
}


void IGMTrackingBase::SetEntryPoint( float x, float y, float z )
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


void IGMTrackingBase::SetImageFiducial( unsigned int index, float *position )
{
	//	std::cout << "Image Fiducial " << index << " set at ( " << position[0] << "," << position[1] << "," << position[2] << ")" << std::endl;
	m_FiducialRegistration.SetDestinationFiducial( index, position );
}



void IGMTrackingBase::SetTrackerFiducial( unsigned int index, float *position )
{
	m_FiducialRegistration.SetSourceFiducial( index, position );
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


void IGMTrackingBase::SetNeedleTipOffset( const float offsetX, const float offsetY, const float offsetZ )
{
	m_NeedleTipOffset[0] = offsetX;
	m_NeedleTipOffset[1] = offsetY;
	m_NeedleTipOffset[2] = offsetZ;
}


void IGMTrackingBase::GetNeedleTipOffset( float& offsetX, float& offsetY, float& offsetZ )
{
	offsetX = m_NeedleTipOffset[0];
	offsetY = m_NeedleTipOffset[1];
	offsetZ = m_NeedleTipOffset[2];
}


void IGMTrackingBase::SetNeedleEndOffset( const float offsetX, const float offsetY, const float offsetZ )
{
	m_NeedleEndOffset[0] = offsetX;
	m_NeedleEndOffset[1] = offsetY;
	m_NeedleEndOffset[2] = offsetZ;
}


void IGMTrackingBase::GetNeedleEndOffset( float& offsetX, float& offsetY, float& offsetZ )
{
	offsetX = m_NeedleEndOffset[0];
	offsetY = m_NeedleEndOffset[1];
	offsetZ = m_NeedleEndOffset[2];
}


void IGMTrackingBase::SetReferenceNeedleTipOffset( const float offsetX, const float offsetY, const float offsetZ )
{
	m_ReferenceNeedleTipOffset[0] = offsetX;
	m_ReferenceNeedleTipOffset[1] = offsetY;
	m_ReferenceNeedleTipOffset[2] = offsetZ;
}


void IGMTrackingBase::GetReferenceNeedleTipOffset( float& offsetX, float& offsetY, float& offsetZ )
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


float IGMTrackingBase::GetNeedleLength( void )
{
	return m_NeedleLength; 
}


	
void IGMTrackingBase::SetNeedleLength( const float val )
{
	m_NeedleLength = val;
}
	


float IGMTrackingBase::GetNeedleRadius( void )
{
	return 	m_NeedleRadius;
}


	
void IGMTrackingBase::SetNeedleRadius( const float val )
{
	m_NeedleRadius = val;
}
	


