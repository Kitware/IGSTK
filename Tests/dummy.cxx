#include "itkImage.h"
#include "DicomImageReader.h"
#include "ImageSliceViewer.h"
#include "RegistrationCT.h"

int main()
{
  typedef signed short   PixelType;

  typedef unsigned char  VisualizationPixelType;

  typedef itk::Image< PixelType, 3 >                  VolumeType;

  typedef itk::Image< PixelType, 2 >                  ImageType;

  typedef ISIS::DicomImageReader< VolumeType >        DicomReaderType;

  typedef ISIS::RegistrationCT< VolumeType >          RegistrationModuleType;

  DicomReaderType dt;
  RegistrationModuleType rm;
  ISIS::ImageSliceViewer  m_AxialViewer;
  return 0;
}
