

#include "IGMTracking.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

int main( int argc,  char * argv [] )
{
	
	IGMTracking app;
	
	try
    {
		//	  Fl::visual(FL_DOUBLE|FL_INDEX);
		app.Show();
		Fl::lock();
		Fl::run();
    }
	catch( std::exception & ex )
    {
		std::cerr << ex.what() << std::endl;
    }
	return 0;
}


