/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMouseTracker_h
#define __igstkMouseTracker_h

#include "igstkTracker.h"

namespace igstk
{

/** \class MouseTracker
 *   \brief Implementation of the Mouse TrackerTool class.
 *
 *   The purpose of this class is to provide a minimal 
 *   implementation of a Tracker. This class is intended 
 *   only for minimal demos and for debugging applications.
 *   It is not intended to be used in a real application to
 *   be deployed in a surgery room. 
 *
 *   \ingroup Trackers
 */
class MouseTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MouseTracker, Tracker )

  typedef Superclass::TransformType           TransformType;

  /** Scaling factor */
  igstkSetMacro( ScaleFactor, double );
  igstkGetMacro( ScaleFactor, double );

protected:

  MouseTracker();

  virtual ~MouseTracker();

  typedef Tracker::ResultType                 ResultType;

  virtual ResultType InternalOpen( void );

  virtual ResultType InternalStartTracking( void );

  virtual ResultType InternalUpdateStatus( void );

  virtual ResultType InternalThreadedUpdateStatus( void );

  virtual ResultType InternalReset( void );

  virtual ResultType InternalStopTracking( void );

  virtual ResultType InternalClose( void );

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( const TrackerToolType * );

  virtual ResultType RemoveTrackerToolFromInternalDataContainers( 
    const TrackerToolType * );

  virtual ResultType AddTrackerToolToInternalDataContainers( 
    const TrackerToolType * );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  MouseTracker(const Self&);  //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  double                              m_ScaleFactor;
};

}

#endif //__igstk_MouseTracker_h_
