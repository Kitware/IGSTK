

#include "Application.h"


int main( int argc,  char * argv [] )
{

  Application app;

  try
    {
    app.Show();
    Fl::run();
    }
  catch( std::exception & ex )
    {
    std::cerr << ex.what() << std::endl;
    }

  return 0;
}


