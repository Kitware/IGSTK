/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkFlockOfBirdsTrackerTool_h
#define __igstkFlockOfBirdsTrackerTool_h

#include "igstkTrackerTool.h"

namespace igstk
{
/** \class FlockOfBirdsTrackerTool
 *
 * \brief A FlockOfBirds-specific TrackerTool class.
 *
 * This class is a for providing FlockOfBirds-specific functionality
 * for TrackerTools, and also to allow compile-time type enforcement
 * for other classes and functions that specifically require
 * an FlockOfBirds tool.
 *
 * \ingroup Tracker
 *
 */

class FlockOfBirdsTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( FlockOfBirdsTrackerTool, TrackerTool )

protected:

  FlockOfBirdsTrackerTool();
  ~FlockOfBirdsTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  FlockOfBirdsTrackerTool(const Self&);    //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

};  


} // namespace igstk


#endif  // __igstk_FlockOfBirdsTrackerTool_h_
