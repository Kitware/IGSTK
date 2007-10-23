/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerToolNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAuroraTrackerToolNew_h
#define __igstkAuroraTrackerToolNew_h

#include "igstkTrackerToolNew.h"

namespace igstk
{
/** \class AuroraTrackerTool
  * \brief An Aurora-specific TrackerTool class.
  *
  * This class is a for providing Aurora-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an Aurora tool.
  *
  * \ingroup Tracker
  *
*/

class AuroraTrackerToolNew : public TrackerToolNew
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AuroraTrackerToolNew, TrackerToolNew )

protected:

  AuroraTrackerToolNew();
  ~AuroraTrackerToolNew();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  AuroraTrackerToolNew(const Self&);    //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

};  


} // namespace igstk


#endif  // __igstk_AuroraTrackerToolNew_h_
