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
FlockOfBirdsTrackerToolNew::FlockOfBirdsTrackerToolNew():m_StateMachine(this)
{
    m_TrackerToolConfigured = false;
    m_PortNumber    = 1;

    // States
    igstkAddStateMacro( Idle );
    igstkAddStateMacro( PortNumberSpecified );

    // Set the input descriptors
    igstkAddInputMacro( ValidPortNumber );
    igstkAddInputMacro( InValidPortNumber );


    // Add transitions
    //
    // Transitions from idle state
    igstkAddTransitionMacro( Idle,
        ValidPortNumber,
        PortNumberSpecified,
        SetPortNumber );

    igstkAddTransitionMacro( Idle,
        InValidPortNumber,
        Idle,
        ReportInvalidPortNumberSpecified );

    // Transitions from MarkerName specified
    igstkAddTransitionMacro( PortNumberSpecified,
        ValidPortNumber,
        PortNumberSpecified,
        ReportInvalidRequest );

    igstkAddTransitionMacro( PortNumberSpecified,
        InValidPortNumber,
        PortNumberSpecified,
        ReportInvalidRequest );

    // Inputs to the state machine
    igstkSetInitialStateMacro( Idle );

    m_StateMachine.SetReadyToRun();
}

/** Destructor */
FlockOfBirdsTrackerToolNew::~FlockOfBirdsTrackerToolNew()
{
}

/** Request set marker name */
void FlockOfBirdsTrackerToolNew::RequestSetPortNumber( unsigned int portNumber )
{
    igstkLogMacro( DEBUG, 
         "igstk::FlockOfBirdsTrackerToolNew::RequestSetPortNumber called ...\n");
    const unsigned int MAXIMUM_PORT_NUMBER = 4;
    const unsigned int MINIMUM_PORT_NUMBER = 1;

  if ( (portNumber > MAXIMUM_PORT_NUMBER) ||
       (portNumber < MINIMUM_PORT_NUMBER) )
    {
    m_StateMachine.PushInput( m_InValidPortNumberInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    this->m_PortNumberToBeSet = portNumber;
    m_StateMachine.PushInput( m_ValidPortNumberInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set valid marker name */ 
void FlockOfBirdsTrackerToolNew::SetPortNumberProcessing( )
{
    igstkLogMacro( DEBUG, 
         "igstk::FlockOfBirdsTrackerToolNew::SetPortNumberProcessing called ...\n");

    this->m_PortNumber = m_PortNumberToBeSet;

    m_TrackerToolConfigured = true;

    //Set the tracker tool unique identifier to the port number. If a
    //channel number is specified a unique identifier will be regenerated
    //using port and channel number information
    std::stringstream identifierStream;
    identifierStream << m_PortNumber;
    this->SetTrackerToolIdentifier( identifierStream.str() );
}

/** Report Invalid bird name*/ 
void FlockOfBirdsTrackerToolNew::ReportInvalidPortNumberSpecifiedProcessing( )
{
    igstkLogMacro( DEBUG, 
    "igstk::FlockOfBirdsTrackerToolNew::ReportInvalidPortNumberSpecifiedProcessing "
         "called ...\n");

    igstkLogMacro( CRITICAL, "Invalid bird name specified ");
}

void FlockOfBirdsTrackerToolNew::ReportInvalidRequestProcessing()
{
   igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker
* tool is configured */ 
bool
FlockOfBirdsTrackerToolNew::CheckIfTrackerToolIsConfigured( ) const
{
   igstkLogMacro( DEBUG, 
          "igstk::FlockOfBirdsTrackerToolNew::CheckIfTrackerToolIsConfigured called...\n");
   return m_TrackerToolConfigured;
}

/** Print Self function */
void FlockOfBirdsTrackerToolNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
    Superclass::PrintSelf(os, indent);

    os << indent << "Port number : "      << m_PortNumber << std::endl;
    os << indent << "TrackerToolConfigured:"
       << m_TrackerToolConfigured << std::endl;
}

}
