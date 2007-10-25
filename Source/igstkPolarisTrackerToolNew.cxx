/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerToolNew.cxx
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

#include "igstkPolarisTrackerToolNew.h"

namespace igstk
{

/** Constructor (initializes Polaris-specific tool values) */
PolarisTrackerToolNew::PolarisTrackerToolNew():m_StateMachine(this)
{

  //Tracker tool initialization flag
  m_TrackerToolInitialized = false;

  // SROM file flag
  m_SROMFileNameSpecified = false;

  // ToolId Specified
  m_ToolIdSpecified = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( WirelessTrackerToolSelected );
  igstkAddStateMacro( WiredTrackerToolSelected );
  igstkAddStateMacro( PortNumberSpecified );
  igstkAddStateMacro( SROMFileNameSpecified );
  igstkAddStateMacro( ToolIdSpecified );

  // Set the input descriptors
  igstkAddInputMacro( SelectWirelessTrackerTool );
  igstkAddInputMacro( SelectWiredTrackerTool); 
  igstkAddInputMacro( ValidPortNumber ); 
  igstkAddInputMacro( InValidPortNumber ); 
  igstkAddInputMacro( ValidSROMFileName ); 
  igstkAddInputMacro( InValidSROMFileName ); 
  igstkAddInputMacro( ValidToolId ); 
  igstkAddInputMacro( InValidToolId ); 

  m_StateMachine.SetReadyToRun();

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           SelectWirelessTrackerTool,
                           WirelessTrackerToolSelected,
                           ReportWirelessTrackerToolSelected );

  igstkAddTransitionMacro( Idle,
                           SelectWiredTrackerTool,
                           WiredTrackerToolSelected,
                           ReportWiredTrackerToolSelected );

  // Transitions from the WiredTrackerToolSelected
  igstkAddTransitionMacro( WiredTrackerToolSelected,
                           ValidPortNumber,
                           PortNumberSpecified,
                           SetPortNumber);

  igstkAddTransitionMacro( WiredTrackerToolSelected,
                           InValidPortNumber,
                           WiredTrackerToolSelected,
                           ReportInValidPortNumberSpecified );

  // Transitions from the WirelessTrackerToolSelected
  igstkAddTransitionMacro( WirelessTrackerToolSelected,
                           ValidPortNumber,
                           PortNumberSpecified,
                           SetPortNumber);

  igstkAddTransitionMacro( WirelessTrackerToolSelected,
                           InValidPortNumber,
                           WirelessTrackerToolSelected,
                           ReportInValidPortNumberSpecified );

  // Transitions from PortNumberSpecified
  igstkAddTransitionMacro( PortNumberSpecified,
                           ValidSROMFileName,
                           SROMFileNameSpecified,
                           SetSROMFileName);

  igstkAddTransitionMacro( PortNumberSpecified,
                           InValidSROMFileName,
                           PortNumberSpecified,
                           ReportInValidSROMFileSpecified);

  // Transitions from SROMFileNameSpecified
  igstkAddTransitionMacro( SROMFileNameSpecified,
                           ValidToolId,
                           ToolIdSpecified,
                           SetToolId);

  igstkAddTransitionMacro( SROMFileNameSpecified,
                           InValidToolId,
                           SROMFileNameSpecified,
                           ReportInValidToolIdSpecified);


  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

}

/** Destructor */
PolarisTrackerToolNew::~PolarisTrackerToolNew()
{
}

/** Print Self function */
void PolarisTrackerToolNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Request the state machine to set the port number */
void PolarisTrackerToolNew::RequestSetPort( unsigned int portNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::RequestSetPort called ...\n");

  if ( portNumber > 255 )
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

/** Request the state machine to set the SROM file name */
void PolarisTrackerToolNew::RequestSetSROMFileName( std::string filename )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::RequestSetSROMFileName called ...\n");

  //FIXME do more filename validation
  //Check if the file exists
  
  if ( filename != "" )
    {
    m_StateMachine.PushInput( m_InValidSROMFileNameInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    m_SROMFileNameToBeSet = filename;
    m_StateMachine.PushInput( m_ValidSROMFileNameInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Request the state machine to set the tool id */
void PolarisTrackerToolNew::RequestSetToolId( std::string toolId )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::RequestSetToolId called ...\n");

  //FIXME DO ToolID verification 
  m_ToolIdToBeSet = toolId;
  m_StateMachine.PushInput( m_ValidToolIdInput );
  m_StateMachine.ProcessInputs();
}

/** Report wireless tracker tool selected */ 
void PolarisTrackerToolNew::ReportWirelessTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportWirelessTrackerToolSelected called ...\n");

  std::cout << "Wireless Tracker tool selected " << std::endl;
}

/** Report wired tracker tool selected */ 
void PolarisTrackerToolNew::ReportWiredTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportWiredTrackerToolSelected called ...\n");

  std::cout << "Wired Tracker tool selected " << std::endl;
}

/** Set valid port number */ 
void PolarisTrackerToolNew::SetPortNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::SetPortNumberProcessing called ...\n");
  m_PortNumber = m_PortNumberToBeSet;
}

/** Report Invalid port number specified */ 
void PolarisTrackerToolNew::ReportInValidPortNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportInValidPortNumberSpecifiedProcessing called ...\n");

}

/** Set valid SROM filename */ 
void PolarisTrackerToolNew::SetSROMFileNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::SetSROMFileNameProcessing called ...\n");

  m_SROMFileName = m_SROMFileNameToBeSet;
}

/** Report Invalid SROM filename specified */ 
void PolarisTrackerToolNew::ReportInValidSROMFileSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportInValidSROMFileSpecifiedProcessing called ...\n");
}

/** Set valid ToolId */ 
void PolarisTrackerToolNew::SetToolIdProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::SetToolIdProcessing called ...\n");

  m_ToolId = m_ToolIdSpecified;
}

/** Report Invalid tool id  specified*/ 
void PolarisTrackerToolNew::ReportInValidToolIdSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TrackerToolNew::ReportInValidToolIdSpecifiedProcessing called ...\n");
}
}
