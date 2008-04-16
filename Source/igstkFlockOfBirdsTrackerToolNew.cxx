/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTrackerToolNew.cxx
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
//Warning about: identifier was truncated to '255' characters 
//in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkFlockOfBirdsTrackerToolNew.h"

namespace igstk
{

/** Constructor (initializes FlockOfBirds-specific tool values) */
FlockOfBirdsTrackerTool::FlockOfBirdsTrackerTool():m_StateMachine(this)
{
    m_TrackerToolConfigured = false;

    // States
    igstkAddStateMacro( Idle );
    igstkAddStateMacro( BirdNameSpecified );

    // Set the input descriptors
    igstkAddInputMacro( ValidBirdName );
    igstkAddInputMacro( InValidBirdName );


    // Add transitions
    //
    // Transitions from idle state
    igstkAddTransitionMacro( Idle,
        ValidBirdName,
        BirdNameSpecified,
        SetBirdName );

    igstkAddTransitionMacro( Idle,
        InValidBirdName,
        Idle,
        ReportInvalidBirdNameSpecified );

    // Transitions from MarkerName specified
    igstkAddTransitionMacro( BirdNameSpecified,
        ValidBirdName,
        BirdNameSpecified,
        ReportInvalidRequest );
    igstkAddTransitionMacro( BirdNameSpecified,
        InValidBirdName,
        BirdNameSpecified,
        ReportInvalidRequest );

    // Inputs to the state machine
    igstkSetInitialStateMacro( Idle );

    m_StateMachine.SetReadyToRun();
}

/** Destructor */
FlockOfBirdsTrackerTool::~FlockOfBirdsTrackerTool()
{
}

/** The "RequestAttachToTracker" method attaches 
 * the tracker tool to a tracker. */
// void RequestAttachToTracker( FlockOfBirdsTracker *  tracker )
// {
//   // This delegation is done only to enforce type matching between
//   // TrackerTool and Tracker. It prevents the user from accidentally 
//   // mix TrackerTools and Trackers of different type;
//   this->TrackerTool::RequestAttachToTracker( tracker );
// }

/** Request set marker name */
void FlockOfBirdsTrackerTool::RequestSetBirdName( const std::string& birdName )
{
    igstkLogMacro( DEBUG, 
         "igstk::FlockOfBirdsTrackerTool::RequestSetBirdName called ...\n");
    if ( birdName == "" )
    {
        m_StateMachine.PushInput( m_InValidBirdNameInput );
        m_StateMachine.ProcessInputs();
    }
    else 
    {
        m_BirdNameToBeSet = birdName;
        m_StateMachine.PushInput( m_ValidBirdNameInput );
        m_StateMachine.ProcessInputs();
    }
}

/** Set valid marker name */ 
void FlockOfBirdsTrackerTool::SetBirdNameProcessing( )
{
    igstkLogMacro( DEBUG, 
         "igstk::FlockOfBirdsTrackerTool::SetBirdNameProcessing called ...\n");

    this->m_BirdName = m_BirdNameToBeSet;

    m_TrackerToolConfigured = true;

    // For MicronTracker, marker name is used as a unique identifier
    this->SetTrackerToolIdentifier( m_BirdName ); 
}

/** Report Invalid marker name*/ 
void FlockOfBirdsTrackerTool::ReportInvalidBirdNameSpecifiedProcessing( )
{
    igstkLogMacro( DEBUG, 
    "igstk::FlockOfBirdsTrackerTool::ReportInvalidBirdNameSpecifiedProcessing "
         "called ...\n");

    igstkLogMacro( CRITICAL, "Invalid bird name specified ");
}

void FlockOfBirdsTrackerTool::ReportInvalidRequestProcessing()
{
   igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker
* tool is configured */ 
bool
FlockOfBirdsTrackerTool::CheckIfTrackerToolIsConfigured( ) const
{
   igstkLogMacro( DEBUG, 
          "igstk::FlockOfBirdsTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
   return m_TrackerToolConfigured;
}

/** Print Self function */
void FlockOfBirdsTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
    Superclass::PrintSelf(os, indent);

    os << indent << "Bird name : "        << m_BirdName << std::endl;
    os << indent << "TrackerToolConfigured:"
       << m_TrackerToolConfigured << std::endl;
}

}
