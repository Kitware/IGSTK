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

class Markers;

namespace igstk
{
/** \class MicronTrackerTool
  * \brief A MicronTracker-specific TrackerTool class.
  *
  * This class is a for providing MicronTracker-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an MicronTrackertool.
  *
  * \ingroup Tracker
  *
  */

class MicronTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MicronTrackerTool, TrackerTool )

  /** Get Tracker tool marker name */
  igstkGetMacro( MarkerName, std::string );

 /** Set tracker tool marker name
 **/
 void RequestSetMarkerName( std::string markerName); 

protected:

  MicronTrackerTool();
  ~MicronTrackerTool();

 /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  /** Get boolean variable to check if the tracker tool is 
   * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured(); 

  MicronTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  std::string m_MarkerName;

  bool m_TrackerToolConfigured;

};  

} // namespace igstk


#endif  // __igstk_MicronTrackerTool_h_
