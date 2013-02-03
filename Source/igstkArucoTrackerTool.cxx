/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkArucoTrackerTool.h,v $
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

#include "igstkArucoTrackerTool.h"
#include "igstkArucoTracker.h"

namespace igstk
{

/** Constructor (configures ArucoTrack-specific tool values)  */
ArucoTrackerTool::ArucoTrackerTool()
                      : m_StateMachine( this )
                      , m_TrackerToolConfigured ( false )
{

  // States
  igstkAddStateMacro ( Idle );
  igstkAddStateMacro ( MarkerNameSpecified );

  // Set the input descriptors
  igstkAddInputMacro ( ValidMarkerName );
  igstkAddInputMacro ( InValidMarkerName );

  // Add transitions
  //
  // Transitions from idle state
  igstkAddTransitionMacro ( Idle,
                           ValidMarkerName,
                           MarkerNameSpecified,
                           SetMarkerName );

  igstkAddTransitionMacro ( Idle,
                           InValidMarkerName,
                           Idle,
                           ReportInvalidMarkerNameSpecified );

  // Transitions from MarkerName specified
  igstkAddTransitionMacro ( MarkerNameSpecified,
                           ValidMarkerName,
                           MarkerNameSpecified,
                           ReportInvalidRequest );

  igstkAddTransitionMacro ( MarkerNameSpecified,
                           InValidMarkerName,
                           MarkerNameSpecified,
                           ReportInvalidRequest );

  // Inputs to the state machine
  igstkSetInitialStateMacro ( Idle );

  this->m_StateMachine.SetReadyToRun ();
}

/** Destructor */
ArucoTrackerTool::~ArucoTrackerTool()
{
}

/** Set tracker tool marker name */
void ArucoTrackerTool::RequestSetMarkerName (
   const unsigned int markerID )
{
  igstkLogMacro( DEBUG,
   "igstk::ArucoTrackerTool::RequestSetMarkerName called ...\n" );

  if ( markerID > 1023 )
    {
    this->m_StateMachine.PushInput( this->m_InValidMarkerNameInput );
    this->m_StateMachine.ProcessInputs();
    }
  else
    {
    this->m_MarkerIDToBeSet = markerID;
    this->m_StateMachine.PushInput( this->m_ValidMarkerNameInput );
    this->m_StateMachine.ProcessInputs();
    }
}

/** Print Self function */
void ArucoTrackerTool::PrintSelf(
   std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Marker ID : "         << this->m_MarkerID << std::endl;
  os << indent << "TrackerToolConfigured:"
     << this->m_TrackerToolConfigured << std::endl;
}

/** Report invalid marker specified */
void ArucoTrackerTool::ReportInvalidMarkerNameSpecifiedProcessing ()
{
  igstkLogMacro( DEBUG,
    "igstk::ArucoTrackerTool::ReportInvalidMarkerNameSpecifiedProcessing \
    called ...\n" );

  igstkLogMacro( CRITICAL, "Invalid marker specified " );
}

/** Report any invalid request to the tool */
void ArucoTrackerTool::ReportInvalidRequestProcessing ()
{
  igstkLogMacro ( WARNING, "ReportInvalidRequestProcessing() called ...\n" );
}

/** Set the marker processing */
void ArucoTrackerTool::SetMarkerNameProcessing ()
{
  igstkLogMacro ( DEBUG,
     "igstk::ArucoTrackerTool::SetMarkerNameProcessing called ...\n" );

  this->m_MarkerID = this->m_MarkerIDToBeSet;
  this->m_TrackerToolConfigured = true;

  // For ArucoTracker, marker ID is used as a unique identifier

  std::stringstream strStream;
  strStream << m_MarkerID;
  std::string markerName = strStream.str();
  SetTrackerToolIdentifier ( markerName );
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if
 * the tracker tool is configured */
bool ArucoTrackerTool::CheckIfTrackerToolIsConfigured () const
{
  igstkLogMacro ( DEBUG,
   "igstk::ArucoTrackerTool::CheckIfTrackerToolIsConfigured called...\n" );
  return this->m_TrackerToolConfigured;
}

}
