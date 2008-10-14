/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerTool.cxx
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

#include "igstkMicronTrackerTool.h"
#include "igstkMicronTracker.h"

namespace igstk
{

/** Constructor (configures Micron-specific tool values) */
MicronTrackerTool::MicronTrackerTool():m_StateMachine(this)
{
  m_TrackerToolConfigured = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( MarkerNameSpecified );

  // Set the input descriptors
  igstkAddInputMacro( ValidMarkerName );
  igstkAddInputMacro( InValidMarkerName );


  // Add transitions
  //
  // Transitions from idle state
  igstkAddTransitionMacro( Idle,
                           ValidMarkerName,
                           MarkerNameSpecified,
                           SetMarkerName );

  igstkAddTransitionMacro( Idle,
                           InValidMarkerName,
                           Idle,
                           ReportInvalidMarkerNameSpecified );

  // Transitions from MarkerName specified
  igstkAddTransitionMacro( MarkerNameSpecified,
                           ValidMarkerName,
                           MarkerNameSpecified,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( MarkerNameSpecified,
                           InValidMarkerName,
                           MarkerNameSpecified,
                           ReportInvalidRequest );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();


}

/** Destructor */
MicronTrackerTool::~MicronTrackerTool()
{
}

/** Request set marker name */
void MicronTrackerTool::RequestSetMarkerName( const std::string& markerName )
{
  igstkLogMacro( DEBUG, 
    "igstk::MicronTrackerTool::RequestSetMarkerName called ...\n");
  if ( markerName == "" )
    {
    m_StateMachine.PushInput( m_InValidMarkerNameInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    m_MarkerNameToBeSet = markerName;
    m_StateMachine.PushInput( m_ValidMarkerNameInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set valid marker name */ 
void MicronTrackerTool::SetMarkerNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::MicronTrackerTool::SetMarkerNameProcessing called ...\n");

  this->m_MarkerName = m_MarkerNameToBeSet;

  m_TrackerToolConfigured = true;

  // For MicronTracker, marker name is used as a unique identifier
  this->SetTrackerToolIdentifier( m_MarkerName ); 
}

/** Report Invalid marker name*/ 
void MicronTrackerTool::ReportInvalidMarkerNameSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::MicronTrackerTool::ReportInvalidMarkerNameSpecifiedProcessing "
    "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid marker name specified ");
}

void MicronTrackerTool::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker
 * tool is configured */ 
bool
MicronTrackerTool::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::MicronTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** Print Self function */
void MicronTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Marker name : "        << m_MarkerName << std::endl;
  os << indent << "TrackerToolConfigured:"
     << m_TrackerToolConfigured << std::endl;
}
}
