#ifndef __ISIS_ApplicationBase__h__
#define __ISIS_ApplicationBase__h__


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "DicomImageReader.h"
#include "RegistrationCT.h"


class ApplicationBase 
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
  
  typedef ISIS::RegistrationCT< VolumeType >          RegistrationModuleType;


public:
  
  ApplicationBase();
  
  virtual ~ApplicationBase();
  
  virtual void SetSeedPoint( double x, double y, double z );
  
  virtual void  GetSeedPoint(double data[3]); 
  
  
  
protected:
  
  double                                   m_SeedPoint[3];
  
  VolumeReaderType::Pointer               m_VolumeReader;
  
  RescaleIntensityFilterType::Pointer     m_RescaleIntensity;

  ITK2VTKAdaptorFilterType::Pointer       m_ITK2VTKAdaptor;
  
  DicomReaderType                         m_DicomVolumeReader;

  VolumeType::ConstPointer                m_LoadedVolume;

  RegistrationModuleType                  m_RegistrationModule;

};


#endif
