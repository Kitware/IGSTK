
#include "itkImage.h"
#include "ImageSliceViewer.h"

#include "vtkImageData.h"
#include "vtkFlRenderWindowInteractor.h"
#include "vtkPNGReader.h"


#include "Fl/Fl_Window.h"

int ImageSliceViewerTest(int argc, char * argv [])
{

  Fl_Window mainWindow( 532, 532 );

  ISIS::ImageSliceViewer  viewer;

  vtkPNGReader * reader = vtkPNGReader::New();
  reader->SetFileName( argv[1] );

  vtkFlRenderWindowInteractor * interactor =
     new vtkFlRenderWindowInteractor( 10, 10, 512, 512, "Viewer" );

  viewer.SetInput( reader->GetOutput() );
  viewer.SetInteractor( interactor );

  reader->Delete();

  mainWindow.end();
  mainWindow.show();
  interactor->show();

  interactor->Initialize();

  Fl::check();
  Fl::check();
  Fl::check();
  Fl::check();

  mainWindow.hide();

  interactor->Delete();

  std::cout << "End of Test " << std::endl;

  return EXIT_SUCCESS;

}

