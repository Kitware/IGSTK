/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkAtracsysEasyTrackTrackerTool.h,v $
  Language:  C++
  Date:      $Date: 2011-05-09 13:29:45 $
  Version:   $Revision: 1.0 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAtracsysEasyTrackTrackerTool_h
#define __igstkAtracsysEasyTrackTrackerTool_h

#include "igstkTrackerTool.h"


namespace igstk
{

class AtracsysEasyTrackTracker;

/** \class AtracsysEasyTrackTrackerTool
  * \brief An AtracsysEasyTrackTracker-specific TrackerTool class.
  *
  *
  * \ingroup Tracker
  *
  */

class AtracsysEasyTrackTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AtracsysEasyTrackTrackerTool, TrackerTool )

  /** The "RequestAttachToTracker" method attaches
   * the tracker tool to a tracker. */
  virtual void RequestAttachToTracker( AtracsysEasyTrackTracker * );

  /** The "RequestSetPortNumber" method sets the port number of the tool */
  void RequestSetPortNumber( unsigned int port );

protected:

  AtracsysEasyTrackTrackerTool();
  virtual ~AtracsysEasyTrackTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  AtracsysEasyTrackTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );

  /** Inputs to the State Machine */

  /** Get boolean variable to check if the tracker tool is
   * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured() const;

  bool            m_TrackerToolConfigured;
};

} // namespace igstk


#endif  // __igstk_AtracsysEasyTrackTrackerTool_h_
