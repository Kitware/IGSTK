/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkCircularSimulatedTracker.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCircularSimulatedTracker_h
#define __igstkCircularSimulatedTracker_h

#include "igstkSimulatedTracker.h"

namespace igstk
{

/** \class CircularSimulatedTracker
 *   \brief A simulated tracker that reports a circular movement
 *
 *   The purpose of this class is to provide a minimal 
 *   implementation of a Tracker. This class is intended 
 *   only for minimal demos and for debugging applications.
 *   It is not intended to be used in a real application to
 *   be deployed in a surgery room. 
 *
 *   \ingroup Trackers
 */
class CircularSimulatedTracker : public SimulatedTracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CircularSimulatedTracker, SimulatedTracker )

  typedef Superclass::TransformType           TransformType;

  /** Scaling factor */
  igstkSetMacro( Radius, double );
  igstkGetMacro( Radius, double );

  /** Angular Speed measured in degrees per second */
  igstkSetMacro( AngularSpeed, double );
  igstkGetMacro( AngularSpeed, double );

protected:

  CircularSimulatedTracker();

  virtual ~CircularSimulatedTracker();

  typedef Tracker::ResultType             ResultType;
  typedef RealTimeClock::TimeStampType    TimeType;

  virtual ResultType InternalOpen( void );

  virtual ResultType InternalStartTracking( void );

  virtual ResultType InternalReset( void );

  virtual ResultType InternalStopTracking( void );

  virtual ResultType InternalClose( void );

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( TrackerToolType * );

  virtual ResultType RemoveTrackerToolFromInternalDataContainers( 
    const TrackerToolType * );


  /** This is the most important method of this class. It computes the
   * transform that will be passed to the TrackerTool. 
   */
  virtual ResultType InternalUpdateStatus( void );

  virtual ResultType InternalThreadedUpdateStatus( void );
  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  CircularSimulatedTracker(const Self&);  //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  double      m_Radius;
  double      m_Angle;
  double      m_AngularSpeed;
  TimeType    m_TimeOfLastUpdate;
};

}

#endif //__igstk_CircularSimulatedTracker_h_
