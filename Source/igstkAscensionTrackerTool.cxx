/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionTrackerTool.cxx
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

#include "igstkAscensionTrackerTool.h"

namespace igstk
{

/** Constructor (initializes Ascension-specific tool values) */
AscensionTrackerTool::AscensionTrackerTool():m_StateMachine(this)
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
AscensionTrackerTool::~AscensionTrackerTool()
{
}

/** Request set marker name */
void AscensionTrackerTool::RequestSetPortNumber( unsigned int portNumber )
{
  igstkLogMacro( DEBUG, 
       "igstk::AscensionTrackerTool::RequestSetPortNumber called ...\n");
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
void AscensionTrackerTool::SetPortNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::AscensionTrackerTool::SetPortNumberProcessing called ...\n");

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
void AscensionTrackerTool::ReportInvalidPortNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
  "igstk::AscensionTrackerTool::ReportInvalidPortNumberSpecifiedProcessing "
       "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid bird name specified ");
}

void AscensionTrackerTool::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker
* tool is configured */ 
bool
AscensionTrackerTool::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::AscensionTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** Print Self function */
void AscensionTrackerTool::PrintSelf( 
  std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Port number : "      << m_PortNumber << std::endl;
  os << indent << "TrackerToolConfigured:"
     << m_TrackerToolConfigured << std::endl;
}

}
