#include <algorithm>

#include "igstkTrajectory.h"

namespace igstk
{
     //a point is on the trajectory if its distance from the trajectory is less 
     //than this threshold
const double Trajectory::POINT_ON_TRAJECTORY_THRESHOLD = 0.5; 

Trajectory::Trajectory()
{
}
  

Trajectory::Trajectory( const PointType &entryPoint,
              const DirectionType &direction,
              const std::vector<PointType> &targets ) 
                throw ( InvalidRequestException )
{
  SetData( entryPoint, direction, targets );
}


Trajectory::Trajectory( const PointType &entryPoint,
              const DirectionType &direction,
              const PointType &target )
{
  SetData( entryPoint, direction, target);
}

  
void 
Trajectory::SetData( const PointType &entryPoint,
                     const DirectionType &direction,
                     const std::vector<PointType> &targets ) 
                       throw ( InvalidRequestException )
{
  DirectionType normalizedDirection = direction;
  normalizedDirection.Normalize();

  std::vector< std::pair<double, unsigned int> > dotProducts;
  
     //validate that all the points are on the linear trajectory and sort them
     //so that the first point is the farthest from the entry point
  int i, n = targets.size();
  for( i=0; i<n; i++ ) 
    {
    DirectionType currentDir = targets[i] - entryPoint;
    double dotProduct = currentDir*normalizedDirection;
    DirectionType perpendicular = currentDir - normalizedDirection*dotProduct;

    if ( perpendicular.GetNorm() > Trajectory::POINT_ON_TRAJECTORY_THRESHOLD )
      throw InvalidRequestException("One of the target points is not on the trajectory");

    dotProducts.push_back( std::make_pair( dotProduct, i ) );
    }
  std::sort( dotProducts.begin(), dotProducts.end() );

  this->m_EntryPoint = entryPoint;
  this->m_Direction = normalizedDirection;
  this->m_Targets.clear();
  for( i=n-1; i>=0; i-- )
    this->m_Targets.push_back( targets[dotProducts[i].second] );
}


void 
Trajectory::SetData( const PointType &entryPoint,
                     const DirectionType &direction,
                     const PointType &target )
{
  this->m_EntryPoint = entryPoint;
  this->m_Direction = direction;
  this->m_Direction.Normalize();
  this->m_Targets.clear();
  this->m_Targets.push_back( target );
}



Trajectory::PointType 
Trajectory::GetEntryPoint() throw ( InvalidRequestException )
{
  if( this->m_Targets.empty() )
    throw InvalidRequestException( std::string("Trajectory not initialized.") );

  return this->m_EntryPoint;
}


Trajectory::DirectionType 
Trajectory::GetDirection() throw ( InvalidRequestException )
{
  if( this->m_Targets.empty() )
    throw InvalidRequestException( std::string("Trajectory not initialized.") );

  return this->m_Direction;
}


void 
Trajectory::GetTargets( std::vector<PointType> &targets ) 
  throw ( InvalidRequestException )
{
  if( this->m_Targets.empty() )
    throw InvalidRequestException( std::string("Trajectory not initialized.") );

  targets.clear();
  targets.insert( targets.begin(), 
                  this->m_Targets.begin(), 
                  this->m_Targets.end() );
}


void 
Trajectory
::Print( std::ostream& os, itk::Indent indent ) const
{
  this->PrintHeader( os, indent ); 
  this->PrintSelf( os, indent.GetNextIndent() );
  this->PrintTrailer( os, indent );
}


void 
Trajectory
::PrintHeader( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "Trajectory" << " (" << this << ")\n";
  os << indent << "RTTI typeinfo: " << typeid( *this ).name() << std::endl;
}


void 
Trajectory
::PrintTrailer( std::ostream& itkNotUsed(os), 
                itk::Indent itkNotUsed(indent) ) const
{
}


void 
Trajectory
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{ 
  os << indent << "Entry Point: "<<this->m_EntryPoint<< std::endl;
  os << indent << "Direction: "<<this->m_Direction<< std::endl;
  os << indent << "Targets:"<< std::endl;
  unsigned int n = this->m_Targets.size();
  for(unsigned int i=0; i<n; i++)
    os << indent << this->m_Targets[i] << std::endl;
}


std::ostream& operator<<( std::ostream& os, const Trajectory& o )
{
  o.Print(os, 0);
  return os;
}



} // end namespace igstk
