#include "OpenIGTLinkTrackingBroadcasterGUI.h"

int main(int argc, char *argv[])
{
  OpenIGTLinkTrackingBroadcasterGUI app;

                        //required by IGSTK for the time stamping used by the tracker
  igstk::RealTimeClock::Initialize();

  app.Show();
  
  while( !app.HasQuitted() )
  {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();
  }


  return EXIT_SUCCESS;
}
