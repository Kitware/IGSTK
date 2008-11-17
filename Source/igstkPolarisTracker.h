/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisTracker_h
#define __igstkPolarisTracker_h

#include "igstkPolarisTrackerTool.h"
#include "igstkNDITracker.h"

namespace igstk
{
/** \class PolarisTracker
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

class PolarisTracker : public NDITracker
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PolarisTracker, NDITracker )

protected:

  PolarisTracker(void);

  virtual ~PolarisTracker(void);

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( const TrackerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified  
   *  frequency is valid for the tracking device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  PolarisTracker(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

};

}

#endif //__igstk_PolarisTracker_h_
