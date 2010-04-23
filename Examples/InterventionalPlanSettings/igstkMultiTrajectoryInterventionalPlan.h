#ifndef __igstkMultiTrajectoryInterventionalPlan_h
#define __igstkMultiTrajectoryInterventionalPlan_h

#include <vector>
#include "igstkInterventionalPlan.h"
#include "igstkTrajectory.h"


namespace igstk 
{
 
/** \class MultiTrajectoryInterventionalPlan 
 *  \brief A class representing a multi trajectory intervention plan.
 * 
 *
 **/

class MultiTrajectoryInterventionalPlan : public InterventionalPlan
{  
public:

  typedef InterventionalPlan  Superclass;

  /** Constructor and destructor */
  MultiTrajectoryInterventionalPlan( const std::string &patientName,
                                     const std::string &studyID,
                                     const std::string &seriesID );
  MultiTrajectoryInterventionalPlan( 
    const MultiTrajectoryInterventionalPlan &other );
  virtual ~MultiTrajectoryInterventionalPlan();

  void AddTrajectory( const Trajectory::PointType &entryPoint,
                      const Trajectory::DirectionType &direction,
                      const std::vector<Trajectory::PointType> &targets ) 
                       throw ( Trajectory::InvalidRequestException );

  void AddTrajectory( const Trajectory::PointType &entryPoint,
                      const Trajectory::DirectionType &direction,
                      const Trajectory::PointType &target ); 
  
  void AddTrajectory( const Trajectory &t );                       

  /**Fill the given vector with the trajectories. All previous data contained in
     the vector is removed.*/
  void GetTrajectories(std::vector<Trajectory> &trajectories);

    /** Method for printing the member variables of this class to an ostream */
  void Print( std::ostream& os, itk::Indent indent ) const;

protected:

    void PrintHeader( std::ostream& os, itk::Indent indent ) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  std::vector<igstk::Trajectory> m_Trajectories;
};

std::ostream& operator<<( std::ostream& os, 
                          const igstk::MultiTrajectoryInterventionalPlan& o );
}

#endif
