#include <itksys/SystemTools.hxx>
//#include <algorithm>
#include "igstkMultiTrajectoryPlanXMLFileReader.h"
#include "igstkMultiTrajectoryInterventionalPlan.h"

namespace igstk
{

void 
MultiTrajectoryPlanXMLFileReader::StartElement( const char * name, 
                                                const char **atts )
{
    //let the super class try to analyze the current tag
  Superclass::StartElement( name, atts );

  if( itksys::SystemTools::Strucmp( name, "trajectory_list" ) == 0 ) 
    {
           //trajectory_list must be nested inside interventional_plan tag
    if( ! this->m_ReadingPlan )
      {
        throw FileFormatException(
        std::string( "The tag \"trajectory_list\" must be nested,\
        inside \"interventional_plan\"." ) );
      }
    this->m_ReadingTrajectoryList = true;
    this->m_Trajectories.clear();
    }
  if( itksys::SystemTools::Strucmp( name, "trajectory" ) == 0 ) 
    {
           //trajectory must be nested inside trajectory_list tag
    if( ! this->m_ReadingTrajectoryList )
      {
        throw FileFormatException(
        std::string( "The tag \"trajectory\" must be nested,\
        inside \"trajectory_list\"." ) );
      }
    this->m_ReadingTrajectory = true;
    this->m_Targets.clear();
    }
  if( itksys::SystemTools::Strucmp( name, "entry_point" ) == 0 ) 
    {
           //entry_point must be nested inside trajectory tag
    if( ! this->m_ReadingTrajectory )
      {
        throw FileFormatException(
        std::string( "The tag \"entry_point\" must be nested,\
        inside \"trajectory\"." ) );
      }
    }
  if( itksys::SystemTools::Strucmp( name, "direction" ) == 0 ) 
    {
           //direction must be nested inside trajectory tag
    if( ! this->m_ReadingTrajectory )
      {
        throw FileFormatException(
        std::string( "The tag \"direction\" must be nested,\
        inside \"trajectory\"." ) );
      }    
    }
  if( itksys::SystemTools::Strucmp( name, "target" ) == 0 ) 
    {
           //target must be nested inside trajectory tag
    if( ! this->m_ReadingTrajectory )
      {
        throw FileFormatException(
        std::string( "The tag \"target\" must be nested,\
        inside \"trajectory\"." ) );
      }    
    }
}


void 
MultiTrajectoryPlanXMLFileReader::EndElement( const char *name )
{
   //let the super class try to analyze the current tag
  Superclass::EndElement( name );
 if( m_ReadingTrajectoryList &&
      itksys::SystemTools::Strucmp( name, "trajectory_list" ) == 0 )
    {
    this->m_ReadingTrajectoryList = false;
    ProcessTrajectoryList();
    }

 if( itksys::SystemTools::Strucmp( name, "trajectory" ) == 0 ) 
    {
      this->m_ReadingTrajectory = false;
      ProcessTrajectory();
    }
 if( itksys::SystemTools::Strucmp( name, "entry_point" ) == 0 ) 
    {      
      ProcessEntryPoint();
      this->m_HaveEntryPoint = true;
    }
 if( itksys::SystemTools::Strucmp( name, "direction" ) == 0 ) 
    {      
      ProcessDirection();
      this->m_HaveDirection = true;
    }
 if( itksys::SystemTools::Strucmp( name, "target" ) == 0 ) 
    {      
      ProcessTarget();      
    }
}


igstk::InterventionalPlan * 
MultiTrajectoryPlanXMLFileReader::GetPlan()
{
  MultiTrajectoryInterventionalPlan *plan = NULL;
  if ( HavePlan() )
    {
    plan = new MultiTrajectoryInterventionalPlan( this->m_PatientName, 
                                                  this->m_studyUID, 
                                                  this->m_seriesUID ); 
    unsigned int n = this->m_Trajectories.size();
    for(unsigned int i=0; i<n ; i++)
      plan->AddTrajectory( this->m_Trajectories[i] );
    }
  return plan;
}


void
MultiTrajectoryPlanXMLFileReader::ProcessTrajectoryList()
{
  if( this->m_Trajectories.empty() )
    {
    throw FileFormatException( "Empty \"trajectory_list\"" );
    }
  this->m_HavePlan = true;
}


void
MultiTrajectoryPlanXMLFileReader::ProcessTrajectory() 
  throw ( FileFormatException )
{
  if( !( this->m_HaveEntryPoint && this->m_HaveDirection && 
             !this->m_Targets.empty() ) )
    {
    throw FileFormatException( std::string( 
      "Missing tag(s) in trajectory section." ) );
    }
  Trajectory t( this->m_EntryPoint, this->m_Direction, this->m_Targets );
  this->m_Trajectories.push_back( t );
  this->m_Targets.clear();
}


void
MultiTrajectoryPlanXMLFileReader::ProcessEntryPoint() 
throw ( FileFormatException )
{
  double x, y, z;

  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>x>>y>>z;

  if( !instr.eof() )
    {
    throw FileFormatException( 
    "Error in entry_point, possibly non numeric values" );
    }
  this->m_EntryPoint[0] = x;
  this->m_EntryPoint[1] = y;
  this->m_EntryPoint[2] = z;
}



void
MultiTrajectoryPlanXMLFileReader::ProcessDirection() 
throw ( FileFormatException )
{
  double x, y, z;

  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>x>>y>>z;

  if( !instr.eof() )
    {
    throw FileFormatException( 
    "Error in direction, possibly non numeric values" );
    }
  this->m_Direction[0] = x;
  this->m_Direction[1] = y;
  this->m_Direction[2] = z;
}


void
MultiTrajectoryPlanXMLFileReader::ProcessTarget() 
throw ( FileFormatException )
{
  double x, y, z;

  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>x>>y>>z;

  if( !instr.eof() )
    {
    throw FileFormatException( 
    "Error in target, possibly non numeric values" );
    }
  Trajectory::PointType target;
  target[0] = x;
  target[1] = y;
  target[2] = z;
  this->m_Targets.push_back( target );
}


} //namespace
