#ifndef __ISIS_LiverTumorSegmentationBase__h__
#define __ISIS_LiverTumorSegmentationBase__h__

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "DicomImageReader.h"
#include "ThresholdLevelSetSegmentationModule.h"
#include "itkImageFileWriter.h"
#include "itkThresholdImageFilter.h"

class LiverTumorSegmentationBase 
{
public:
  
//  typedef signed short   PixelType;
  typedef unsigned char   PixelType;

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

  typedef itk::ThresholdImageFilter< VolumeType >     ThresholdFilterType;
  
  typedef ISIS::ThresholdLevelSetSegmentationModule::InputImageType    InputImageType;

  typedef ISIS::ThresholdLevelSetSegmentationModule::OutputImageType   OutputImageType;

  /** Writer that will send the content of the segmented image to
      a file.  This is expected to be a binary mask */
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

public:
  
  LiverTumorSegmentationBase();
  
  virtual ~LiverTumorSegmentationBase();
  
  virtual void SetSeedPoint( float x, float y, float z );
  
  virtual void GetSeedPoint(float data[3]); 
  
  virtual bool DoSegmentation( void );
  
  virtual bool DoThreshold( float lower, float upper );

  virtual void WriteSegmentedVolume( const char *fname );

  virtual void WriteBinarySegmentedVolume( const char *fname );

  virtual bool Load( const char *filename );

protected:
  
  float                                   m_SeedPoint[3];

  int                                     m_SeedIndex[3];

  float									  m_SeedValue;

  VolumeReaderType::Pointer               m_VolumeReader;
  
  RescaleIntensityFilterType::Pointer     m_RescaleIntensity;

  ThresholdFilterType::Pointer            m_ThresholdVolumeFilter;

  ITK2VTKAdaptorFilterType::Pointer       m_ITK2VTKAdaptor;
  
  RescaleIntensityFilterType::Pointer     m_OverlayVolumeRescaleIntensity;

  ITK2VTKAdaptorFilterType::Pointer       m_OverlayVolumeITK2VTKAdaptor;
  
  DicomReaderType                         m_DicomVolumeReader;

  VolumeType::ConstPointer                m_LoadedVolume;

  VolumeType::ConstPointer                m_SegmentedVolume;

  VolumeType::ConstPointer                m_ThresholdedVolume;

  ISIS::ThresholdLevelSetSegmentationModule	  m_LiverTumorSegmentationModule;

  char									  m_MessageString[256];

  WriterType::Pointer					  m_Writer;

  VolumeReaderType::Pointer	temp_VolumeReader;
};


#endif
