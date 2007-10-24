#include "PivotCalibrationFLTKWidgetExample.h"
#include "igstkRealTimeClock.h"

      
int main(int argc, char *argv[])
{
  PivotCalibrationFLTKWidgetExample app;

                        //required by IGSTK for the time stamping used by the tracker
  igstk::RealTimeClock::Initialize();

  app.Show();  
  return Fl::run();
}
