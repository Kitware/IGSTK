#include "igstkTrajectory.h"
#include "igstkMultiTrajectoryInterventionalPlan.h"
#include "igstkMultiTrajectoryPlanXMLFileReader.h"

#include <vector>
#include <algorithm>

int main( int argc, char *argv[] )
{
  if( argc != 2 )
    {
    std::cerr<<"Wrong number of arguments.\n";
    std::cerr<<"Usage: "<<argv[0]<<" plan_file.xml\n";
    return EXIT_FAILURE;
    }
  igstk::MultiTrajectoryPlanXMLFileReader::Pointer planReader;
    
  planReader = igstk::MultiTrajectoryPlanXMLFileReader::New();
  planReader->SetFilename( argv[1] );
  try 
  {       //read the xml file
    planReader->GenerateOutputInformation();
      //xml reading is successful (no exception generation) even 
      //when the file is empty, need to check that the data was
      //actually read
    if( planReader->HavePlan() )
      {
      igstk::MultiTrajectoryInterventionalPlan *plan = 
        dynamic_cast<igstk::MultiTrajectoryInterventionalPlan *>( planReader->GetPlan() );
            //print the plan, includes some RTTI info, patient name, study and 
            //series ID in addition to the trajectory data
      //std::cout<<(*plan);

            //get the trajectory data and print it
      std::vector<igstk::Trajectory> trajectories;
      plan->GetTrajectories( trajectories );
          //user responsible for releasing the memory of the pointer returned from GetPlan()
      delete plan;
                    
            //a trajectory consists of an entry point, a direction and a set of 
            //targets
      igstk::Trajectory::PointType entryPoint;
      igstk::Trajectory::DirectionType direction;
      std::vector<igstk::Trajectory::PointType> targets;
       
       for( unsigned int i=0; i<trajectories.size(); i++ )
         {
           entryPoint = trajectories[i].GetEntryPoint();
           direction = trajectories[i].GetDirection();
           trajectories[i].GetTargets( targets );
           std::cout<<"Trajectory ["<<i<<"]"<<":\n";
           std::cout<<"\tEntry point: "<<entryPoint<<"\n";
           std::cout<<"\tDirection: "<<direction<<"\n";
           std::cout<<"\tTargets:\n";
           for( unsigned int j=0; j<targets.size(); j++ ) 
            std::cout<<"\t\t"<<targets[j]<<"\n";
         }

      }
    //an itk exception is thrown if the xml is malformed, and a 
    //FileFormatException is thrown if the data format is incorrect.
    //both are decendants of std::exception 
  } 
  catch( std::exception &e ) 
    {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
