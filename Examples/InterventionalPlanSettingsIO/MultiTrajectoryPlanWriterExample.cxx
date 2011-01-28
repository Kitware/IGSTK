#include "igstkTrajectory.h"
#include "igstkMultiTrajectoryInterventionalPlan.h"
#include "igstkMultiTrajectoryPlanXMLFileWriter.h"

int main( int , char *[] )
{
  igstk::Trajectory::PointType entryPoint, t;
  igstk::Trajectory::DirectionType direction;
  std::vector<igstk::Trajectory::PointType> targets;
  
  igstk::MultiTrajectoryInterventionalPlan interventionalPlan("George T. Own", "1","2");

  entryPoint[0] = -165.672;  
  entryPoint[1] =-151.672; 
  entryPoint[2] = -101;

  direction[0] =0.7581;  
  direction[1] =0.3790;  
  direction[2] = -0.5307;
  
  t[0] = -123.5980;
  t[1] = -130.6350;
  t[2] = -130.4520;
  targets.push_back( t );

  t[0] = -113.9320;
  t[1] = -125.8020;
  t[2] = -137.2180;
  targets.push_back( t );

  t[0] = -132.1260;
  t[1] = -134.8990;
  t[2] = -124.4820;
  targets.push_back( t );

  interventionalPlan.AddTrajectory( entryPoint, direction, targets );

  targets.clear();
  
  entryPoint[0] = -165.672;  
  entryPoint[1] = -151.672; 
  entryPoint[2] = -101;
          
  direction[0] = 0.7514;  
  direction[1] = 0.1600;  
  direction[2] = -0.6401;
     
  t[0] = -127.9130;
  t[1] = -143.6300;
  t[2] = -133.1650;
  targets.push_back( t );
     
  t[0] = -114.3870;
  t[1] = -140.7490;
  t[2] = -144.6870;
  targets.push_back( t );

  interventionalPlan.AddTrajectory( entryPoint, direction, targets );

  igstk::MultiTrajectoryPlanXMLFileWriter::Pointer planWriter = 
    igstk::MultiTrajectoryPlanXMLFileWriter::New();

  planWriter->SetFilename("testPlan.xml");
  planWriter->SetObject( &interventionalPlan );

  if( !planWriter->WriteFile() )
    return EXIT_FAILURE;

  return EXIT_SUCCESS;

  //std::cout<<interventionalPlan<<"\n";
}
