#include "itkImage.h"
#include "DicomImageReader.h"


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"


inline void println(char *s) { std::cout << s << std::endl; }

int DicomImageReaderTest(int argc, char * argv [])
{
  typedef signed short   PixelType;

  if( argc < 3 ) {
	  std::cerr << "Usage: ctest -V -R DicomImageReader";
	  return EXIT_FAILURE;
  }

  typedef itk::Image<PixelType, 2> myImage;
  itk::ImageFileReader<myImage>::Pointer reader  = itk::ImageFileReader<myImage>::New();
  reader->SetFileName(argv[1]);
  try{
    reader->Update();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << "exception in file reader " << std::endl;
    std::cerr << e.GetDescription() << std::endl;
    std::cerr << e.GetLocation() << std::endl;
    return EXIT_FAILURE;
  }



  typedef unsigned char WritePixelType;
  typedef itk::Image< WritePixelType, 2 > WriteImageType;
  typedef itk::RescaleIntensityImageFilter<myImage, WriteImageType > RescaleFilterType;
  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );
  
  typedef itk::ImageFileWriter< WriteImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );

  rescaler->SetInput( reader->GetOutput() );
  writer->SetInput( rescaler->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;

}

