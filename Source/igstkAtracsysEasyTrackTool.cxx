/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkAtracsysEasyTrackTool.cxx,v $
  Language:  C++
  Date:      $Date: 2011-05-09 13:29:45 $
  Version:   $Revision: 1.0 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <math.h>

#include "igstkAtracsysEasyTrackTool.h"
#include "igstkAtracsysEasyTrack.h"

namespace igstk
{

/** Constructor (configures AtracsysEasyTrack-specific tool values) */
AtracsysEasyTrackTool::AtracsysEasyTrackTool():m_StateMachine(this)
{
  m_TrackerToolConfigured = true;

  // States
  igstkAddStateMacro( Idle );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();
}

/** Destructor */
AtracsysEasyTrackTool::~AtracsysEasyTrackTool()
{
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker
 * tool is configured */
bool
AtracsysEasyTrackTool::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG,
    "igstk::PolarisTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** The "RequestAttachToTracker" method attaches
 * the tracker tool to a tracker. */
void AtracsysEasyTrackTool::RequestAttachToTracker(
                                                AtracsysEasyTrack *  tracker )
{
  // This delegation is done only to enforce type matching between
  // TrackerTool and Tracker. It prevents the user from accidentally
  // mix TrackerTools and Trackers of different type;
  this->TrackerTool::RequestAttachToTracker( tracker );
}

/** Print Self function */
void AtracsysEasyTrackTool::PrintSelf(
                                  std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Marker name : "        << /*m_RigidBodyName <<*/ std::endl;
  os << indent << "TrackerToolConfigured:"
     << m_TrackerToolConfigured << std::endl;

}

void AtracsysEasyTrackTool::RequestSetPortNumber( int port )
{
    char strPort[128];
    sprintf(strPort, "%d", port);

    this->SetTrackerToolIdentifier( std::string(strPort) );
}

}
