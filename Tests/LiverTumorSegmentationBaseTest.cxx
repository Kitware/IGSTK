
#include "LiverTumorSegmentationBase.h"

int LiverTumorSegmentationBaseTest(int argc, char * argv [])
{
  if( argc < 3 ) {
	  std::cerr << "Usage: ctest -V -R LiverTumorSegmentationBaseTest";
	  return EXIT_FAILURE;
  }

	LiverTumorSegmentationBase app;

	try
	{
		app.Load( argv[1] );
		app.SetSeedPoint( 108.22, 126.45, 160.82 );
		app.DoSegmentation();
		app.WriteSegmentedVolume( argv[2] );
	}
	catch( std::exception & ex )
	{
		std::cerr << ex.what() << std::endl;
		std::cerr << "exception in Liver Tumor Segmentation Test " << std::endl;
		return EXIT_FAILURE;
	}

  return EXIT_SUCCESS;

}

