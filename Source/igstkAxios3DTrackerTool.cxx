/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkAxios3DTracker.h,v $
  Language:  C++
  Date:      $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

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

#include "igstkAxios3DTrackerTool.h"
#include "igstkAxios3DTracker.h"

namespace igstk
{

/** Constructor (configures Axios3DTrack-specific tool values)  */
Axios3DTrackerTool::Axios3DTrackerTool()
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
Axios3DTrackerTool::~Axios3DTrackerTool()
{
}

/** Set tracker tool marker name */
void Axios3DTrackerTool::RequestSetMarkerName (
   const std::string & markerName)
{
  igstkLogMacro(DEBUG,
   "igstk::Axios3DTrackerTool::RequestSetMarkerName called ...\n");

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
void Axios3DTrackerTool::PrintSelf(
   std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Marker name : "         << this->m_MarkerName << std::endl;
  os << indent << "TrackerToolConfigured:"
     << this->m_TrackerToolConfigured << std::endl;
}

/** Report invalid marker specified */
void Axios3DTrackerTool::ReportInvalidMarkerNameSpecifiedProcessing ()
{
  igstkLogMacro( DEBUG,
    "igstk::Axios3DTrackerTool::ReportInvalidMarkerNameSpecifiedProcessing \
    called ...\n");

  this->InvokeEvent(InvalidMarkerNameErrorEvent());
}

/** Report any invalid request to the tool */
void Axios3DTrackerTool::ReportInvalidRequestProcessing ()
{
  igstkLogMacro (WARNING, "ReportInvalidRequestProcessing() called ...\n");

  this->InvokeEvent(InvalidRequestErrorEvent());
}

/** Set the marker processing */
void Axios3DTrackerTool::SetMarkerNameProcessing ()
{
  igstkLogMacro (DEBUG,
     "igstk::Axios3DTrackerTool::SetMarkerNameProcessing called ...\n");

  this->m_MarkerName = this->m_MarkerNameToBeSet;
  this->m_TrackerToolConfigured = true;

  // For Axios3DTracker, marker name is used as a unique identifier
  SetTrackerToolIdentifier (this->m_MarkerName);
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if
 * the tracker tool is configured */
bool Axios3DTrackerTool::CheckIfTrackerToolIsConfigured () const
{
  igstkLogMacro (DEBUG,
   "igstk::Axios3DTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return this->m_TrackerToolConfigured;
}

}
