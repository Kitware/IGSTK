/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker2.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAuroraTracker2_h
#define __igstkAuroraTracker2_h

#include "igstkAuroraTrackerTool2.h"
#include "igstkNDITracker.h"

namespace igstk
{
/** \class AuroraTracker2
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

class AuroraTracker2 : public NDITracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AuroraTracker2, NDITracker )

protected:

  AuroraTracker2(void);

  virtual ~AuroraTracker2(void);

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( const TrackerToolType * );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  AuroraTracker2(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented
};

}

#endif //__igstk_AuroraTracker2_h_
