/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMicronTrackerTool_h
#define __igstkMicronTrackerTool_h

#include "igstkTrackerTool.h"

namespace igstk
{
/** \class MicronTrackerTool
  * \brief A MicronTracker-specific TrackerTool class.
  *
  * This class is a for providing Polaris-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an Polaris tool.
  *
  * \ingroup Tracker
  *
  */

class MicronTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MicronTrackerTool, TrackerTool )

  /** Load marker template */
  void LoadMarkerTemplate( std::string filename );

protected:

  MicronTrackerTool();
  ~MicronTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  MicronTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  //marker template directory
  std::string m_MarkerTemplateDirectory;
};  


} // namespace igstk


#endif  // __igstk_MicronTrackerTool_h_
