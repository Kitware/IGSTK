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

	virtual void SetTargetPoint( double x, double y, double z );

	virtual void SetEntryPoint( double x, double y, double z );

	virtual void SetNumberOfFiducials( unsigned int number );

	virtual void SetImageFiducial( unsigned int index, double *position );
	
	virtual void SetImageFiducial( unsigned int index ) = NULL;
	
	virtual void SetTrackerFiducial( unsigned int index, double *position);
	
	virtual void SetTrackerFiducial( unsigned int index ) = NULL;
	
	virtual unsigned int GetNumberOfFiducials( void );

	virtual bool RegisterImageWithTrackerAndComputeRMSError( void );
	
	virtual void SetNeedleTipOffset( const double offsetX, const double offsetY, const double offsetZ );

	virtual void GetNeedleTipOffset( double& offsetX, double& offsetY, double& offsetZ );

	virtual void SetNeedleEndOffset( const double offsetX, const double offsetY, const double offsetZ );

	virtual void GetNeedleEndOffset( double& offsetX, double& offsetY, double& offsetZ );

	virtual double GetNeedleLength( void );
	
	virtual void SetNeedleLength( const double val );
	
	virtual double GetNeedleRadius( void );
	
	virtual void SetNeedleRadius( const double val );

	virtual void SetReferenceNeedleTipOffset( const double offsetX, const double offsetY, const double offsetZ );

	virtual void GetReferenceNeedleTipOffset( double& offsetX, double& offsetY, double& offsetZ );
	
	virtual void SetUseReferenceNeedleOption( const bool option );

	virtual bool GetUseReferenceNeedleOption( void );

protected:
	
	virtual void SetTargetPoint( const double point[3] );	

	virtual void SetEntryPoint( const double point[3] );	

	virtual void PrintMessage( char *msg );
	
protected:
	
	VolumeReaderType::Pointer               m_VolumeReader;
	
	RescaleIntensityFilterType::Pointer     m_RescaleIntensity;
	
	ITK2VTKAdaptorFilterType::Pointer       m_ITK2VTKAdaptor;
	
	DicomReaderType                         m_DicomVolumeReader;
	
	VolumeType::ConstPointer                m_LoadedVolume;

	double									m_TargetPoint[3];

	double									m_EntryPoint[3];

	FiducialRegistration					m_FiducialRegistration;

	char			m_StringBuffer[256];	// used as a scratch pad for printing messages, etc.
	
	// Main Needle Parameters

	double			m_NeedlePosition[3]; 

	double			m_NeedleTipOffset[3];		// Offset of tool tip head from the coil

	double			m_NeedleEndOffset[3];		// Offset of tool end head from the coil

	double			m_NeedleLength, m_NeedleRadius;

	// Reference Needle Parameters

	bool			m_UseReferenceNeedle;

	double			m_ReferenceNeedlePosition[3]; 

	double			m_ReferenceNeedleTipOffset[3];		
};


#endif
