#include "itkImage.h"
#include "DicomImageReader.h"


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"


inline void println(char *s) { std::cout << s << std::endl; }

int DicomImageReaderTestDirectory(int argc, char * argv [])
{

  typedef signed short   PixelType;


  typedef unsigned char  VisualizationPixelType;
  typedef itk::Image< PixelType, 3 >                  VolumeType;
  typedef itk::Image< PixelType, 2 >                  ImageType;
  typedef ISIS::DicomImageReader< VolumeType >        DicomReaderType;

  std::vector<std::string>  m_SeriesUIDs;

  DicomReaderType reader;

  if( argc < 5 ) {
	  std::cerr << "Usage: ctest -V -R DicomImageReaderTestDirectory";
	  return EXIT_FAILURE;
  }

  reader.SetDirectory(argv[1]);
  m_SeriesUIDs = reader.GetSeriesUIDs();

  int n = atoi(argv[2]);
  for(int i=0;i<n;i++){
	  if ( strcmp(m_SeriesUIDs[i].c_str(),argv[i+3]) ) return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}

