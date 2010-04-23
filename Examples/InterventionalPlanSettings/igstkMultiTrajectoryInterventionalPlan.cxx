#include "igstkMultiTrajectoryInterventionalPlan.h"


namespace igstk
{

MultiTrajectoryInterventionalPlan::MultiTrajectoryInterventionalPlan(
  const std::string &patientName,
  const std::string &studyID,
  const std::string &seriesID ) : 
    InterventionalPlan(patientName, studyID, seriesID) 
{
}


MultiTrajectoryInterventionalPlan::MultiTrajectoryInterventionalPlan(
  const MultiTrajectoryInterventionalPlan &other ) : InterventionalPlan( other )
{
  this->m_Trajectories.insert( this->m_Trajectories.begin(),
                               other.m_Trajectories.begin(),
                               other.m_Trajectories.end() );
}


MultiTrajectoryInterventionalPlan::~MultiTrajectoryInterventionalPlan() 
{
}


void 
MultiTrajectoryInterventionalPlan::AddTrajectory(
  const Trajectory::PointType &entryPoint,
  const Trajectory::DirectionType &direction,
  const std::vector<Trajectory::PointType> &targets ) 
    throw ( Trajectory::InvalidRequestException )
{
  this->m_Trajectories.push_back( Trajectory( entryPoint, direction, targets ) );
}


void 
MultiTrajectoryInterventionalPlan::AddTrajectory(
  const Trajectory::PointType &entryPoint,
  const Trajectory::DirectionType &direction,
  const Trajectory::PointType &target )    
{
  this->m_Trajectories.push_back( Trajectory( entryPoint, direction, target ) );
}


void 
MultiTrajectoryInterventionalPlan::AddTrajectory( const Trajectory &t )
{
  this->m_Trajectories.push_back( t );
}


void 
MultiTrajectoryInterventionalPlan::GetTrajectories(
  std::vector<igstk::Trajectory> &trajectories)
{
  trajectories.clear();
  trajectories.insert( trajectories.begin(), 
                       this->m_Trajectories.begin(),
                       this->m_Trajectories.end() );

}


void 
MultiTrajectoryInterventionalPlan
::Print( std::ostream& os, itk::Indent indent ) const
{
  this->PrintHeader( os, indent ); 
  this->PrintSelf( os, indent.GetNextIndent() );
  this->PrintTrailer( os, indent );
}


void 
MultiTrajectoryInterventionalPlan
::PrintHeader( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "Multi Trajectory Plan" << " (" << this << ")\n";
  os << indent << "RTTI typeinfo: " << typeid( *this ).name() << std::endl;
}


void 
MultiTrajectoryInterventionalPlan
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{ 
  if( !this->m_Trajectories.empty() )
    {
    os << indent << "Trajectories:\n";
    int n = this->m_Trajectories.size();
    for(int i=0; i<n; i++)
      this->m_Trajectories[i].Print( os, indent );
    }
  Superclass::PrintSelf( os, indent.GetNextIndent() );
}


std::ostream& operator<<( std::ostream& os, 
                          const MultiTrajectoryInterventionalPlan& o )
{
  o.Print(os, 0);
  return os;
}

} // end namespace igstk
