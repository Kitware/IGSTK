
#include "itkImage.h"
#include "ImageSliceViewer.h"

#include "vtkImageData.h"


int ImageSliceViewerTest(int argc, char * argv [])
{


  ISIS::ImageSliceViewer  viewer;

  vtkImageData * image = vtkImageData::New();

  viewer.SetInput( image );


  return EXIT_SUCCESS;

}

