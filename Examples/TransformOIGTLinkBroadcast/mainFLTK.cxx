#include "OIGTLinkTrackingBroadcasterGUI.h"

int main(int argc, char *argv[])
{
  OIGTLinkTrackingBroadcasterGUI app;

                        //required by IGSTK for the time stamping used by the tracker
  igstk::RealTimeClock::Initialize();

  app.Show();
  
  return Fl::run();
}
