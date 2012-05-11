/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICertusTrackerTool.cxx
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

#include <math.h>

#include "igstkNDICertusTrackerTool.h"
#include "igstkNDICertusTracker.h"

namespace igstk
{

/** Constructor (configures Polaris-specific tool values) */
NDICertusTrackerTool::NDICertusTrackerTool():m_StateMachine(this)
{
  m_TrackerToolConfigured = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( RigidBodyNameSpecified );

  // Set the input descriptors
  igstkAddInputMacro( ValidRigidBodyName );
  igstkAddInputMacro( InValidRigidBodyName );


  // Add transitions
  //
  // Transitions from idle state
  igstkAddTransitionMacro( Idle,
                           ValidRigidBodyName,
                           RigidBodyNameSpecified,
                           SetRigidBodyName );

  igstkAddTransitionMacro( Idle,
                           InValidRigidBodyName,
                           Idle,
                           ReportInvalidRigidBodyNameSpecified );

  // Transitions from MarkerName specified
  igstkAddTransitionMacro( RigidBodyNameSpecified,
                           ValidRigidBodyName,
                           RigidBodyNameSpecified,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( RigidBodyNameSpecified,
                           InValidRigidBodyName,
                           RigidBodyNameSpecified,
                           ReportInvalidRequest );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();
}

/** Destructor */
NDICertusTrackerTool::~NDICertusTrackerTool()
{

}

/** Request set rigidbody name */
void NDICertusTrackerTool::RequestSetRigidBodyName( const std::string& rigidBodyName )
{
  igstkLogMacro( DEBUG, "igstk::NDICertusTrackerTool::RequestSetRigidBodyName called ...\n");
  if ( rigidBodyName == "" )
    {
    m_StateMachine.PushInput( m_InValidRigidBodyNameInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    m_RigidBodyNameToBeSet = rigidBodyName;
    m_StateMachine.PushInput( m_ValidRigidBodyNameInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set valid RigidBody name */ 
void NDICertusTrackerTool::SetRigidBodyNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::NDICertusTrackerTool::SetRigidBodyNameProcessing called ...\n");

  this->m_RigidBodyName = m_RigidBodyNameToBeSet;

  m_TrackerToolConfigured = true;

  // For NDICertusTracker, RigidBody name is used as a unique identifier
  this->SetTrackerToolIdentifier( m_RigidBodyName ); 
}


/** Report Invalid RigidBody name*/ 
void NDICertusTrackerTool::ReportInvalidRigidBodyNameSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::NDICertusTrackerTool::ReportInvalidRigidBodyNameSpecifiedProcessing "
    "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid RigidBody name specified ");
}

void NDICertusTrackerTool::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}


/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker
 * tool is configured */ 
bool
NDICertusTrackerTool::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** The "RequestAttachToTracker" method attaches 
 * the tracker tool to a tracker. */
void NDICertusTrackerTool::RequestAttachToTracker( NDICertusTracker *  tracker )
{
  // This delegation is done only to enforce type matching between
  // TrackerTool and Tracker. It prevents the user from accidentally 
  // mix TrackerTools and Trackers of different type;
  this->TrackerTool::RequestAttachToTracker( tracker );
}

/** Print Self function */
void NDICertusTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);


  os << indent << "Marker name : "        << m_RigidBodyName << std::endl;
  os << indent << "TrackerToolConfigured:"
     << m_TrackerToolConfigured << std::endl;

}

}
