
#include "itkImage.h"
#include "ImageSliceViewer.h"

int ImageSliceViewerTest(int argc, char * argv [])
{


  ImageSliceViewer  viewer;

  vtkImageData * image = vtkImageData::New();

  viewer->SetInput( image );


  return EXIT_SUCCESS;

}

