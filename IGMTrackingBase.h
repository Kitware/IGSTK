#ifndef __ISIS_IGMTrackingBase__h__
#define __ISIS_IGMTrackingBase__h__


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "DicomImageReader.h"
#include "FiducialRegistration.h"

class IGMTrackingBase 
{
public:
	
	typedef signed short   PixelType;
	
	typedef unsigned char  VisualizationPixelType;
	
	typedef itk::Image< PixelType, 3 >                  VolumeType;
	
	typedef itk::Image< PixelType, 2 >                  ImageType;
	
	typedef itk::ImageFileReader< VolumeType >          VolumeReaderType;
	
	typedef ISIS::DicomImageReader< VolumeType >        DicomReaderType;
	
	typedef itk::Image< VisualizationPixelType, 3 >     VisualizationVolumeType;
	
	typedef itk::RescaleIntensityImageFilter< 
		VolumeType,
		VisualizationVolumeType >   RescaleIntensityFilterType;
	
	typedef itk::ImageToVTKImageFilter< 
		VisualizationVolumeType >   ITK2VTKAdaptorFilterType;
	
public:
	
	IGMTrackingBase();
	
	virtual ~IGMTrackingBase();

	virtual void SetTargetPoint( float x, float y, float z );

	virtual void SetEntryPoint( float x, float y, float z );

	virtual void SetNumberOfFiducials( unsigned int number );

	virtual void SetImageFiducial( unsigned int index, float *position );
	
	virtual void SetImageFiducial( unsigned int index ) = NULL;
	
	virtual void SetTrackerFiducial( unsigned int index, float *position);
	
	virtual void SetTrackerFiducial( unsigned int index ) = NULL;
	
	virtual unsigned int GetNumberOfFiducials( void );

	virtual bool RegisterImageWithTrackerAndComputeRMSError( void );
	
	virtual void SetNeedleTipOffset( const float offsetX, const float offsetY, const float offsetZ );

	virtual void GetNeedleTipOffset( float& offsetX, float& offsetY, float& offsetZ );

	virtual void SetNeedleEndOffset( const float offsetX, const float offsetY, const float offsetZ );

	virtual void GetNeedleEndOffset( float& offsetX, float& offsetY, float& offsetZ );

	virtual float GetNeedleLength( void );
	
	virtual void SetNeedleLength( const float val );
	
	virtual float GetNeedleRadius( void );
	
	virtual void SetNeedleRadius( const float val );

	virtual void SetReferenceNeedleTipOffset( const float offsetX, const float offsetY, const float offsetZ );

	virtual void GetReferenceNeedleTipOffset( float& offsetX, float& offsetY, float& offsetZ );
	
	virtual void SetUseReferenceNeedleOption( const bool option );

	virtual bool GetUseReferenceNeedleOption( void );

protected:
	
	virtual void SetTargetPoint( const float point[3] );	

	virtual void SetEntryPoint( const float point[3] );	

	virtual void PrintMessage( char *msg );
	
protected:
	
	VolumeReaderType::Pointer               m_VolumeReader;
	
	RescaleIntensityFilterType::Pointer     m_RescaleIntensity;
	
	ITK2VTKAdaptorFilterType::Pointer       m_ITK2VTKAdaptor;
	
	DicomReaderType                         m_DicomVolumeReader;
	
	VolumeType::ConstPointer                m_LoadedVolume;

	float									m_TargetPoint[3];

	float									m_EntryPoint[3];

	FiducialRegistration					m_FiducialRegistration;

	char			m_StringBuffer[256];	// used as a scratch pad for printing messages, etc.
	
	// Main Needle Parameters

	float			m_NeedlePosition[3]; 

	float			m_NeedleTipOffset[3];		// Offset of tool tip head from the coil

	float			m_NeedleEndOffset[3];		// Offset of tool end head from the coil

	float			m_NeedleLength, m_NeedleRadius;

	// Reference Needle Parameters

	bool			m_UseReferenceNeedle;

	float			m_ReferenceNeedlePosition[3]; 

	float			m_ReferenceNeedleTipOffset[3];		
};


#endif
