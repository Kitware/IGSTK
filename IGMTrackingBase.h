#ifndef __ISIS_IGMTrackingBase__h__
#define __ISIS_IGMTrackingBase__h__

#include "itkAffineTransform.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkIsolatedConnectedImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkNeighborhoodConnectedImageFilter.h"
#include "itkRawImageIO.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "DicomImageReader.h"
#include "FiducialRegistration.h"
#include "vtkImageBlend.h"
#include "vtkImageResample.h"
#include "vtkImageReslice.h"
#include "vtkImageMapToRGBA.h"
#include "vtkLookupTable.h"

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

	typedef itk::ImageFileWriter< VolumeType >			RawWriterType;

	typedef itk::RawImageIO< PixelType, 3 >				RawImageIOType;
	
	typedef itk::RescaleIntensityImageFilter< 
		VolumeType,
		VisualizationVolumeType >   RescaleIntensityFilterType;
	
	typedef itk::ImageToVTKImageFilter< 
		VolumeType >   ITK2VTKAdaptorFilterType;

  typedef itk::ConfidenceConnectedImageFilter< 
    VolumeType, VolumeType> ConfidenceConnectedFilterType;

  typedef itk::ConnectedThresholdImageFilter< 
    VolumeType, VolumeType> ConnectedThresholdFilterType;

  typedef itk::NeighborhoodConnectedImageFilter< 
    VolumeType, VolumeType> NeighborhoodConnectedFilterType;

  typedef itk::IsolatedConnectedImageFilter< 
    VolumeType, VolumeType> IsolatedConnectedFilterType;

  typedef itk::ResampleImageFilter<
    VolumeType, VolumeType> ResampleImageFilterType;

  typedef itk::AffineTransform <double, 3> TransformType;

  typedef itk::LinearInterpolateImageFunction<VolumeType, double> LInterpolateType;

  typedef itk::NearestNeighborInterpolateImageFunction<VolumeType, double> NNInterpolateType;

public:
	virtual void OnUpdateRegParameters();
	virtual void OnUpdateSegParameters();
	virtual void SetInputData(int pipeline);
	virtual void RenderAllWindow();
  virtual void RenderSectionWindow();
  double m_FusionOpacity;
  int m_Pipeline;
	virtual void OnUpdateOpacity(double opa);
	virtual void OnUpdateProbeParameters();
	virtual void OnUpdateLayout();
	
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
	
public:
	
	VolumeReaderType::Pointer               m_VolumeReader;

  VolumeReaderType::Pointer               m_FusionVolumeReader;
	
	RescaleIntensityFilterType::Pointer     m_RescaleIntensity;

  RescaleIntensityFilterType::Pointer     m_FusionRescaleIntensity;
	
	ITK2VTKAdaptorFilterType::Pointer       m_ITK2VTKAdaptor;

  ITK2VTKAdaptorFilterType::Pointer       m_FusionITK2VTKAdaptor;
	
  ConfidenceConnectedFilterType::Pointer  m_ConfidenceConnectedFilter;

  ConnectedThresholdFilterType::Pointer   m_ConnectedThresholdFilter;

  NeighborhoodConnectedFilterType::Pointer    m_NeighborhoodConnectedFilter;

  IsolatedConnectedFilterType::Pointer    m_IsolatedConnectedFilter;

  ResampleImageFilterType::Pointer        m_ResampleImagePreFilter;

  ResampleImageFilterType::Pointer        m_ResampleImagePostFilter;

  ResampleImageFilterType::Pointer        m_ResampleImageFusionFilter;

  TransformType::Pointer                  m_Transform;

  LInterpolateType::Pointer               m_LInterpolate;

  NNInterpolateType::Pointer              m_NNInterpolate;
	
  DicomReaderType                         m_DicomVolumeReader;

  DicomReaderType                         m_FusionDicomVolumeReader;
	
	VolumeType::ConstPointer                m_LoadedVolume;

//<<<<<<< IGMTrackingBase.h
  VolumeType::ConstPointer                m_FusionVolume;
/*
  vtkImageBlend*                  m_ImageBlend;

  vtkImageResample*                 m_ImageResample;

  vtkImageReslice*                  m_ImageReslice;

  vtkImageMapToRGBA*                m_ImageMap;

  vtkLookupTable*                   m_LUT;
*/
//	double									m_TargetPoint[3];
//=======
	double									m_TargetPoint[3];
//>>>>>>> 1.2

	double									m_EntryPoint[3];

	FiducialRegistration					m_FiducialRegistration;

	char			m_StringBuffer[256];	// used as a scratch pad for printing messages, etc.
	
	// Main Needle Parameters

	double			m_NeedlePosition[3]; 

	double			m_NeedleTipOffset[3];		// Offset of tool tip head from the coil

	double			m_NeedleEndOffset[3];		// Offset of tool end head from the coil

	double			m_NeedleLength, m_NeedleRadius;

  double			m_RegNeedleTipOffset[3];		// Offset of tool tip head from the coil

	double			m_RegNeedleEndOffset[3];		// Offset of tool end head from the coil

	double			m_RegNeedleLength, m_RegNeedleRadius;

	// Reference Needle Parameters

	bool			m_UseReferenceNeedle;

	double			m_ReferenceNeedlePosition[3]; 

	double			m_ReferenceNeedleTipOffset[3];		
public:

  virtual void OnRecord(void)
  {
  }

  virtual void OnEvaluatePosition(double x, double y, double z){};
};


#endif
