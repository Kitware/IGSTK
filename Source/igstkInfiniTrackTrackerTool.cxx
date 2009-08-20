/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkInfiniTrackTrackerTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkInfiniTrackTrackerTool.h"
#include "igstkInfiniTrackTracker.h"

namespace igstk
{

/** Constructor (configures InfiniTrack-specific tool values)  */
InfiniTrackTrackerTool::InfiniTrackTrackerTool()
                      : m_StateMachine(this)
                      , m_TrackerToolConfigured (false)
{

  // States
  igstkAddStateMacro (Idle);
  igstkAddStateMacro (MarkerNameSpecified);

  // Set the input descriptors
  igstkAddInputMacro (ValidMarkerName);
  igstkAddInputMacro (InValidMarkerName);

  // Add transitions
  //
  // Transitions from idle state
  igstkAddTransitionMacro (Idle,
                           ValidMarkerName,
                           MarkerNameSpecified,
                           SetMarkerName);

  igstkAddTransitionMacro (Idle,
                           InValidMarkerName,
                           Idle,
                           ReportInvalidMarkerNameSpecified);

  // Transitions from MarkerName specified
  igstkAddTransitionMacro (MarkerNameSpecified,
                           ValidMarkerName,
                           MarkerNameSpecified,
                           ReportInvalidRequest);

  igstkAddTransitionMacro (MarkerNameSpecified,
                           InValidMarkerName,
                           MarkerNameSpecified,
                           ReportInvalidRequest);

  // Inputs to the state machine
  igstkSetInitialStateMacro (Idle);

  this->m_StateMachine.SetReadyToRun ();
}

/** Destructor */
InfiniTrackTrackerTool::~InfiniTrackTrackerTool()
{
}


/** Set tracker tool marker name */
void InfiniTrackTrackerTool::RequestSetMarkerName (
   const std::string & markerName)
{
  igstkLogMacro(DEBUG, 
   "igstk::InfiniTrackTrackerTool::RequestSetMarkerName called ...\n");

  if (markerName == "")
    {
    this->m_StateMachine.PushInput( this->m_InValidMarkerNameInput );
    this->m_StateMachine.ProcessInputs();
    } 
  else 
    {
    this->m_MarkerNameToBeSet = markerName;
    this->m_StateMachine.PushInput( this->m_ValidMarkerNameInput );
    this->m_StateMachine.ProcessInputs();
    }
}

/** Print Self function */
void InfiniTrackTrackerTool::PrintSelf( 
   std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Marker name : "         << this->m_MarkerName << std::endl;
  os << indent << "TrackerToolConfigured:" 
     << this->m_TrackerToolConfigured << std::endl;
}

/** Report invalid marker specified */
void InfiniTrackTrackerTool::ReportInvalidMarkerNameSpecifiedProcessing ()
{
  igstkLogMacro( DEBUG, 
    "igstk::InfiniTrackTrackerTool::ReportInvalidMarkerNameSpecifiedProcessing \
    called ...\n");

  igstkLogMacro( CRITICAL, "Invalid marker specified ");
}

/** Report any invalid request to the tool */
void InfiniTrackTrackerTool::ReportInvalidRequestProcessing ()
{
  igstkLogMacro (WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** Set the marker processing */
void InfiniTrackTrackerTool::SetMarkerNameProcessing ()
{
  igstkLogMacro (DEBUG, 
     "igstk::InfiniTrackTrackerTool::SetMarkerNameProcessing called ...\n");

  this->m_MarkerName = this->m_MarkerNameToBeSet;
  this->m_TrackerToolConfigured = true;

  // For InfiniTrackTracker, marker name is used as a unique identifier
  SetTrackerToolIdentifier (this->m_MarkerName); 
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if 
 * the tracker tool is configured */
bool InfiniTrackTrackerTool::CheckIfTrackerToolIsConfigured () const
{
  igstkLogMacro (DEBUG, 
   "igstk::InfiniTrackTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return this->m_TrackerToolConfigured;
}

}
