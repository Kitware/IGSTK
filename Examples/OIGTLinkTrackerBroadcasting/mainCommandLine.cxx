#include "OIGTLinkTrackingBroadcaster.h"

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
    OIGTLinkTrackingBroadcaster *b = new OIGTLinkTrackingBroadcaster( std::string( argv[1] ) );
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
