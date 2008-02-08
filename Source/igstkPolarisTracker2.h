/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTracker2.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisTracker2_h
#define __igstkPolarisTracker2_h

#include "igstkSerialCommunication.h"
#include "igstkNDICommandInterpreter.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkNDITracker.h"

namespace igstk
{
/** \class PolarisTracker2
  * \brief Provides support for the Polaris optical tracker.
  *
  * This class provides an interface to the Polaris line of
  * optical tracking systems, including the Vicra and the
  * Spectra.  These devices are manufactured by
  * Northern Digital Inc. of Waterloo, Ontario, Canada.
  *
  * Tracking of the silvered markers individually is not possible,
  * the markers must be configured as a tool before they can
  * be tracked.
  *
  * \ingroup Tracker
  * \ingroup NDITracker
  *
  */

class PolarisTracker2 : public NDITracker
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PolarisTracker2, NDITracker )

protected:

  PolarisTracker2(void);

  virtual ~PolarisTracker2(void);

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( const TrackerToolType * );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  PolarisTracker2(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

};

}

#endif //__igstk_PolarisTracker2_h_
