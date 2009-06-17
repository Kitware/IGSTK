/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAuroraTracker_h
#define __igstkAuroraTracker_h

#include "igstkAuroraTrackerTool.h"
#include "igstkNDITracker.h"

namespace igstk
{
/** \class AuroraTracker
  * \brief Provides support for the AURORA magnetic tracker.
  *
  * This class provides an interface to the Aurora magnetic
  * tracking system, which is manufactured by
  * Northern Digital Inc. in Waterloo, Ontario, Canada.
  * 
  * The use of two 5DOF tools on a single port is supported.
  * In order to use the Aurora in this configuration, a 
  * splitter for the port is required, as well as an SROM
  * file that can work with the two tools in question. 
  *
  * \ingroup Tracker
  * \ingroup NDITracker
  *
  */

const unsigned int MAX_PORT_NUMBER = 12;
const unsigned int MAX_WIRED_PORT_NUMBER = 4;
const unsigned int NUMBER_OF_ATTEMPTS = 256;
const unsigned int MAX_CHANNEL_NUMBER = 1;

class AuroraTracker : public NDITracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AuroraTracker, NDITracker )

  /** typedefs for the tool */
  typedef igstk::AuroraTrackerTool      AuroraTrackerToolType;
      
  /** allow simulated tracker test to maintain code coverage
      w/o communication, via Set/Get. **/
   igstkSetMacro( SimulatedTestMaintainCoverage, bool );   
   igstkGetMacro( SimulatedTestMaintainCoverage, bool );
      
protected:

  AuroraTracker(void);

  virtual ~AuroraTracker(void);

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( const TrackerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified  
   *  frequency is valid for the tracking device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );
 
  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

  /** Remove tracker tool entry from internal containers */ 
  virtual ResultType RemoveTrackerToolFromInternalDataContainers(
                                  const TrackerToolType * trackerTool ); 

private:

  AuroraTracker(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  // Keep track of initialized port with spliter
  int                               m_HasSpliter[MAX_WIRED_PORT_NUMBER];
  int                               m_SpliterHandle[MAX_WIRED_PORT_NUMBER][2];
  bool                              m_SimulatedTestMaintainCoverage;
};

}

#endif //__igstk_AuroraTracker_h_
