
#include "MotionViewer.h"
#include "vtkFlRenderWindowInteractor.h"

#include "Fl/Fl_Window.h"

int MotionViewerTest(int argc, char * argv [])
{
	ISIS::MotionViewer  viewer;
	
//	Fl_Window mainWindow( 600, 600 );	
	
	vtkFlRenderWindowInteractor * interactor =
		new vtkFlRenderWindowInteractor( 10, 10, 512, 512, "Viewer" );
	
//	mainWindow.end();
//	mainWindow.show();
	interactor->show();
	
	viewer.SetInteractor( interactor );
	
	interactor->Initialize();

	viewer.SetupCamera();

//	for (int i = -20; i < 20; i++)
//	{
//		viewer.UpdateMotion(i);
//	}
	
	Fl::check();
	Fl::check();
	Fl::check();
	Fl::check();
	
//	mainWindow.hide();
	
	interactor->Delete();
	
	cout << "End of Test " << endl;
	
	return EXIT_SUCCESS;
	
}