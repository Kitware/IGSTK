#include "igstkMultiTrajectoryInterventionalPlan.h"
#include "igstkMultiTrajectoryPlanXMLFileWriter.h"

namespace igstk
{

int
MultiTrajectoryPlanXMLFileWriter::WritePlan( std::ofstream &out )
{
  MultiTrajectoryInterventionalPlan *plan = 
    dynamic_cast<MultiTrajectoryInterventionalPlan *>( this->m_InputObject );
  if( plan==NULL )
    return 0;

   std::vector<Trajectory> trajectories;
   plan->GetTrajectories( trajectories );
   out<<"<trajectory_list>\n";
   for( int i=0; i<trajectories.size(); i++ )
    WriteTrajectory( trajectories[i], out );
   out<<"</trajectory_list>\n";
  return 1;
}

void 
MultiTrajectoryPlanXMLFileWriter::WriteTrajectory( Trajectory &trajectory, 
                                                   std::ofstream &out )
{
  Trajectory::PointType entryPoint;
  Trajectory::DirectionType direction;
  std::vector<Trajectory::PointType> targets;

  out<<"<trajectory>\n";
  entryPoint = trajectory.GetEntryPoint();
  out<<"<entry_point>"<<entryPoint[0]<<"\t"<<entryPoint[1]<<"\t"<<entryPoint[2];        
  out<<"</entry_point>\n";
  
  direction = trajectory.GetDirection();
  out<<"<direction>"<<direction[0]<<"\t"<<direction[1]<<"\t"<<direction[2];        
  out<<"</direction>\n";

  trajectory.GetTargets( targets );
  for( int i=0; i<targets.size(); i++ )
    {
    out<<"<target>"<<(targets[i])[0]<<"\t"<<(targets[i])[1]<<"\t"<<(targets[i])[2];        
    out<<"</target>\n";
    }    
  out<<"</trajectory>\n";
}

} //namespace
