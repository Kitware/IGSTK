#include "itkImage.h"
#include "DicomImageReader.h"

int DicomImageReaderTest(int argc, char * argv [])
{
  typedef signed short   PixelType;

  typedef unsigned char  VisualizationPixelType;

  typedef itk::Image< PixelType, 3 >                  VolumeType;

  typedef itk::Image< PixelType, 2 >                  ImageType;

  typedef ISIS::DicomImageReader< VolumeType >        DicomReaderType;

  DicomReaderType reader;

  if( argc > 1 ) 
    {
    reader.SetDirectory( argv[1] );



    }

  return EXIT_SUCCESS;

}

