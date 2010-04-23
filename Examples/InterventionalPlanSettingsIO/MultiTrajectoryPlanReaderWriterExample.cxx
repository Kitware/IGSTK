#include "igstkTrajectory.h"
#include "igstkMultiTrajectoryInterventionalPlan.h"
#include "igstkMultiTrajectoryPlanXMLFileReader.h"
#include "igstkMultiTrajectoryPlanXMLFileWriter.h"

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


      igstk::MultiTrajectoryPlanXMLFileWriter::Pointer planWriter = 
        igstk::MultiTrajectoryPlanXMLFileWriter::New();

      std::string outputFileName;    
      std::string fullFileName = std::string( argv[1] );
      size_t index = fullFileName.find_last_of('.');
                    //output file name is the same as input file name with a 
                   //postfix of _modified.xml
      outputFileName = index == std::string::npos ?
        fullFileName + std::string("_modified.xml") :
        fullFileName.substr(0,index) + std::string("_modified.xml");

      planWriter->SetFilename( outputFileName );
      planWriter->SetObject( plan );

      if( !planWriter->WriteFile() )
        return EXIT_FAILURE;
          //user responsible for releasing the memory of the pointer returned from GetPlan()
      delete plan;
      
      return EXIT_SUCCESS;
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
