#include "OpenIGTLinkTrackingBroadcaster.h"

int main( int argc, char *argv[] )
{
  if( argc!=2 )
  {
    std::cerr<<"Usage: "<<argv[0];
    std::cerr<<" tracker_xml_configuration_file.\n";
    return EXIT_FAILURE;
  }

  try 
  {
    std::string s = argv[1] ;
    OpenIGTLinkTrackingBroadcaster *b = new OpenIGTLinkTrackingBroadcaster( s );
    b->StartTracking();
    while( 1 ) 
    {
      igstk::PulseGenerator::Sleep( 10 );
      igstk::PulseGenerator::CheckTimeouts();
    }
    return EXIT_SUCCESS;    
  }
  catch( std::exception &e )
  {    
    std::cerr<<e.what()<<"\n";
    return EXIT_FAILURE;
  }
}
