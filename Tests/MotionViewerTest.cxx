
#include "MotionViewer.h"
#include "vtkFlRenderWindowInteractor.h"

#include "Fl/Fl_Window.h"

int MotionViewerTest(int argc, char * argv [])
{
	ISIS::MotionViewer  viewer;
	
	vtkFlRenderWindowInteractor * interactor =
		new vtkFlRenderWindowInteractor(0, 0, 512, 512, "MotionViewer" );
	
	interactor->show();
	
	viewer.SetInteractor( interactor );
	
	interactor->Initialize();

	viewer.SetupCamera();

	for (int i = -20; i < 20; i++)
	{
		viewer.UpdateMotion(i);
	}
	
	interactor->Delete();
	
	cout << "End of Test " << endl;
	
	return EXIT_SUCCESS;
	
}