
#include "itkImage.h"
#include "RegistrationCT.h"

int RegistrationCTTest(int argc, char * argv [])
{

  typedef signed short   PixelType;

  typedef itk::Image< PixelType, 3 >                  VolumeType;

  typedef ISIS::RegistrationCT< VolumeType >        RegistratorType;

  RegistratorType registrator;

  VolumeType::Pointer fixedImage  = VolumeType::New();
  VolumeType::Pointer movingImage = VolumeType::New();

  registrator.SetFixedImage(  fixedImage  );
  registrator.SetMovingImage( movingImage );

  VolumeType::ConstPointer resampledImage = registrator.GetOutput();

  return EXIT_SUCCESS;

}

