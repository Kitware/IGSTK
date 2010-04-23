#ifndef __igstkTrajectory_h
#define __igstkTrajectory_h

#include "itkVector.h"
#include "itkPoint.h"
#include <vector>

namespace igstk 
{
 
/** \class Trajectory 
 *  \brief A class representing a linear trajectory with one or more target 
 *         points along the path.
 * 
 **/

class Trajectory 
{
public:

  /** This exception class only serves as a container for an error message. 
   *  The C++ std::exception implementation has no constructor with string input 
   *  when conforming to the language standard (it does in the visual
   *  studio implementation which breaks the standard). This class just adds the
   *  functionality without breaking the standard.
   */
 class InvalidRequestException : public std::exception
    {
    public:
    /**
     * Construct an exception with a specifc message.
     */
    InvalidRequestException(const std::string &errorMessage) 
      {
      this->m_ErrorMessage = errorMessage;
      }

    /**
     * Virtual destructor.
     */
    virtual ~InvalidRequestException() throw() {}

    /**
     * Get the error message.
     */
    virtual const char* what() const throw() 
      {
      return m_ErrorMessage.c_str();
      }
    private:
    std::string m_ErrorMessage;
    };

  typedef ::itk::Vector<double, 3>    DirectionType;
  typedef ::itk::Point<double, 3>     PointType;
  
  /** Construct an empty trajectory**/
  Trajectory();

     /** Construct a trajectory with multiple targets. If one of the target 
         points is not on the trajectory an exception is thrown.**/
  Trajectory( const PointType &entryPoint,
              const DirectionType &direction,
              const std::vector<PointType> &targets ) 
                throw ( InvalidRequestException );

       /** Construct a trajectory with a single target**/
  Trajectory( const PointType &entryPoint,
              const DirectionType &direction,
              const PointType &target );

  virtual ~Trajectory() {}

  /** Set the trajectory to a new one having multiple targets. If one of the 
      target points is not on the trajectory an exception is thrown.**/
  void SetData( const PointType &entryPoint,
                const DirectionType &direction,
                const std::vector<PointType> &targets ) 
                  throw ( InvalidRequestException );

   /** Set the trajectory to a new one.*/
  void SetData( const PointType &entryPoint,
                const DirectionType &direction,
                const PointType &target );

  /** Return the trajectories entry point.*/
  PointType GetEntryPoint() throw ( InvalidRequestException );

  /** Return the normalized trajectory direction.*/
  DirectionType GetDirection() throw ( InvalidRequestException );

  /** Get the target points along this trajectory. The given vector is cleared 
   *  from all previous contents. The points are sorted according to distance 
   *  from the entry point, farthest to nearest. 
   */
  void GetTargets( std::vector<PointType> &targets )
    throw ( InvalidRequestException );
  
  /** Method for printing the member variables of this class to an ostream */
  void Print( std::ostream& os, itk::Indent indent ) const;


protected:

  void PrintHeader( std::ostream& os, itk::Indent indent ) const;

  void PrintTrailer( std::ostream& itkNotUsed(os), 
                     itk::Indent itkNotUsed(indent) ) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  PointType              m_EntryPoint;
  DirectionType          m_Direction;
  std::vector<PointType> m_Targets;

       //a point is on the trajectory if its distance from the trajectory is less 
     //than this threshold
  static const double POINT_ON_TRAJECTORY_THRESHOLD;
};

std::ostream& operator<<( std::ostream& os, const igstk::Trajectory& o );
}

#endif
