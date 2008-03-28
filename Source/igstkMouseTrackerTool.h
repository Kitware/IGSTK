/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMouseTrackerTool_h
#define __igstkMouseTrackerTool_h

#include "igstkTrackerTool.h"

namespace igstk
{

class MouseTracker;

/** \class MouseTrackerTool
  * \brief A MouseTracker-specific TrackerTool class.
  *
  * \ingroup Tracker
  *
  */

class MouseTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MouseTrackerTool, TrackerTool )

  typedef std::string       MouseNameType;

  /** Get Tracker tool mouse name */
  igstkGetStringMacro( MouseName );

  /** Set tracker tool mouse name */
  virtual void RequestSetMouseName( const MouseNameType & MouseName ); 

protected:

  MouseTrackerTool();
  ~MouseTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  /** Get boolean variable to check if the tracker tool is 
   * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured() const; 

  MouseTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  MouseNameType   m_MouseName;

  bool m_TrackerToolConfigured;

};  

} // namespace igstk


#endif  // __igstk_MouseTrackerTool_h_
