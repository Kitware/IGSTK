/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkSimulatedTracker.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSimulatedTracker_h
#define __igstkSimulatedTracker_h

#include "igstkTracker.h"

namespace igstk
{

/** \class SimulatedTracker
 *   \brief Implementation of a class that simulates a tracker.
 *
 *   The purpose of this class is to provide the behavior
 *   of a Tracker but under controlled conditions. This class
 *   is mainly intended as a helper device for testing other
 *   components of an IGS system. This simulated tracker generate
 *   transforms that correspond to a pre-programmed path in space.
 *
 *   By connecting this tracker to an object, you can validate
 *   whether the visualization, timing and interaction elements
 *   of the IGS application are behaving as expected or not.
 *
 *   \ingroup Trackers
 */
class SimulatedTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( SimulatedTracker, Tracker )

  typedef Superclass::TransformType           TransformType;


protected:

  SimulatedTracker();

  virtual ~SimulatedTracker();

  typedef Tracker::ResultType                 ResultType;

  virtual ResultType InternalOpen( void );

  virtual ResultType InternalStartTracking( void );

  virtual ResultType InternalReset( void );

  virtual ResultType InternalStopTracking( void );

  virtual ResultType InternalClose( void );

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( const TrackerToolType * );

  virtual ResultType RemoveTrackerToolFromInternalDataContainers( 
                                                   const TrackerToolType * );

  virtual ResultType AddTrackerToolToInternalDataContainers( 
                                                   const TrackerToolType * );

  /** This is the most important method of the SimulatedTracker family.
   *  This method must be overloaded in derived classes in order to
   *  provide a particular pattern of movement.
   */
  virtual ResultType InternalUpdateStatus( void );
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  SimulatedTracker(const Self&);  //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

}

#endif //__igstk_SimulatedTracker_h_
