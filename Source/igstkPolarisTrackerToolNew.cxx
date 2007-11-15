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
#include <sstream>

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

  // Wireless tool selected
  m_WirelessTrackerToolSelected  = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( WirelessTrackerToolSelected );
  igstkAddStateMacro( WiredTrackerToolSelected );
  igstkAddStateMacro( WiredTrackerToolPortNumberSpecified );
  igstkAddStateMacro( WirelessTrackerToolSROMFileNameSpecified );
  igstkAddStateMacro( WiredTrackerToolSROMFileNameSpecified );
  igstkAddStateMacro( WirelessTrackerToolToolIdSpecified );
  igstkAddStateMacro( WiredTrackerToolToolIdSpecified );


  // Set the input descriptors
  igstkAddInputMacro( SelectWirelessTrackerTool );
  igstkAddInputMacro( SelectWiredTrackerTool); 
  igstkAddInputMacro( ValidPortNumber ); 
  igstkAddInputMacro( InValidPortNumber ); 
  igstkAddInputMacro( ValidSROMFileName ); 
  igstkAddInputMacro( InValidSROMFileName ); 
  igstkAddInputMacro( ValidToolId ); 
  igstkAddInputMacro( InValidToolId ); 


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
                           WiredTrackerToolPortNumberSpecified,
                           SetPortNumber);

  igstkAddTransitionMacro( WiredTrackerToolSelected,
                           InValidPortNumber,
                           WiredTrackerToolSelected,
                           ReportInValidPortNumberSpecified );

  // Transitions from the WirelessTrackerToolSelected
  igstkAddTransitionMacro( WirelessTrackerToolSelected,
                           ValidSROMFileName,
                           WirelessTrackerToolSROMFileNameSpecified,
                           SetSROMFileName);

  igstkAddTransitionMacro( WirelessTrackerToolSelected,
                           InValidSROMFileName,
                           WirelessTrackerToolSelected,
                           ReportInValidSROMFileSpecified );

  // Transitions from WiredTrackerToolPortNumberSpecified
  igstkAddTransitionMacro( WiredTrackerToolPortNumberSpecified,
                           ValidSROMFileName,
                           WiredTrackerToolSROMFileNameSpecified,
                           SetSROMFileName);

  igstkAddTransitionMacro( WiredTrackerToolPortNumberSpecified,
                           InValidSROMFileName,
                           WiredTrackerToolPortNumberSpecified,
                           ReportInValidSROMFileSpecified);

  // Transitions from WiredTrackerToolSROMFileNameSpecified
  igstkAddTransitionMacro( WiredTrackerToolSROMFileNameSpecified,
                           ValidToolId,
                           WiredTrackerToolToolIdSpecified,
                           SetToolId);

  igstkAddTransitionMacro( WiredTrackerToolSROMFileNameSpecified,
                           InValidToolId,
                           WiredTrackerToolSROMFileNameSpecified,
                           ReportInValidToolIdSpecified);

  // Transitions from WirelessTrackerToolSROMFileNameSpecified
  igstkAddTransitionMacro( WirelessTrackerToolSROMFileNameSpecified,
                           ValidToolId,
                           WirelessTrackerToolToolIdSpecified,
                           SetToolId);

  igstkAddTransitionMacro( WirelessTrackerToolSROMFileNameSpecified,
                           InValidToolId,
                           WirelessTrackerToolSROMFileNameSpecified,
                           ReportInValidToolIdSpecified);


  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();

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

/** Request the state machine to select wired tracker tool model*/
void PolarisTrackerToolNew::RequestSelectWiredTrackerTool( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::RequestSelectWiredTrackerTool called ...\n");

  m_StateMachine.PushInput( m_SelectWiredTrackerToolInput );
  m_StateMachine.ProcessInputs();
}

/** Request the state machine to select wireless tracker tool model*/
void PolarisTrackerToolNew::RequestSelectWirelessTrackerTool( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::RequestSelectWirelessTrackerTool called ...\n");

  m_StateMachine.PushInput( m_SelectWirelessTrackerToolInput );
  m_StateMachine.ProcessInputs();
}
 
/** Request the state machine to set the port number */
void PolarisTrackerToolNew::RequestSetPortNumber( unsigned int portNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::RequestSetPortNumber called ...\n");

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
    "igstk::PolarisTrackerToolNew::RequestSetSROMFileName called ...\n");

  //FIXME do more filename validation
  //Check if the file exists
  
  if ( filename == "" )
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

/** Check if the tool is wireless type*/
bool PolarisTrackerToolNew::IsToolWirelessType( )
{
  return m_WirelessTrackerToolSelected;
}

/** Check if the SROM filename is specified */
bool PolarisTrackerToolNew::IsSROMFileNameSpecified( )
{
  return m_SROMFileNameSpecified;
}

/** Get SROM filename */ 
std::string PolarisTrackerToolNew::GetSROMFileName( )
{
  return m_SROMFileName;
}

/** Get Port number */ 
unsigned int PolarisTrackerToolNew::GetPortNumber( )
{
  return m_PortNumber;
}

/** Request the state machine to set the tool id */
void PolarisTrackerToolNew::RequestSetToolId( std::string toolId )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::RequestSetToolId called ...\n");

  //FIXME DO ToolID verification 
  m_ToolIdToBeSet = toolId;
  m_StateMachine.PushInput( m_ValidToolIdInput );
  m_StateMachine.ProcessInputs();
}

/** Report wireless tracker tool selected */ 
void PolarisTrackerToolNew::ReportWirelessTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::ReportWirelessTrackerToolSelected called ...\n");

  std::cout << "Wireless Tracker tool selected " << std::endl;

  m_WirelessTrackerToolSelected = true;
}

/** Report wired tracker tool selected */ 
void PolarisTrackerToolNew::ReportWiredTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::ReportWiredTrackerToolSelected called ...\n");

  std::cout << "Wired Tracker tool selected " << std::endl;
}

/** Set valid port number */ 
void PolarisTrackerToolNew::SetPortNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::SetPortNumberProcessing called ...\n");
  m_PortNumber = m_PortNumberToBeSet;
  
  std::stringstream identifierStream;
  identifierStream << m_PortNumber;
  
  // For polaris tracker, port number could be used as a unique identifier
  this->SetTrackerToolIdentifier( identifierStream.str() );

  // if the tool is wired type and if the port number is set, we can
  // "declare" the tracker tool initialized. The user can also specify
  // SROM file and tool id.  
  if ( ! m_WirelessTrackerToolSelected )
    {
    m_TrackerToolInitialized = true;
    }
}

/** Report Invalid port number specified */ 
void PolarisTrackerToolNew::ReportInValidPortNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::ReportInValidPortNumberSpecifiedProcessing called ...\n");

  std::cerr << "Invalid Port Number specified " << std::endl;
}

/** Set valid SROM filename */ 
void PolarisTrackerToolNew::SetSROMFileNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::SetSROMFileNameProcessing called ...\n");

  m_SROMFileName = m_SROMFileNameToBeSet;
  m_SROMFileNameSpecified = true;

  //At this point, port number and SROM file are specified. For wireless
  //tracker tools this is the minimum requirement. The tool can be "declared"
  //intialized.
  m_TrackerToolInitialized = true;
}

/** Report Invalid SROM filename specified */ 
void PolarisTrackerToolNew::ReportInValidSROMFileSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::ReportInValidSROMFileSpecifiedProcessing called ...\n");

  std::cerr << "Invalid SROM file specified " << std::endl;
}

/** Set valid ToolId */ 
void PolarisTrackerToolNew::SetToolIdProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::SetToolIdProcessing called ...\n");

  m_ToolId = m_ToolIdToBeSet;
  m_ToolIdSpecified = true;
}

/** Report Invalid tool id  specified*/ 
void PolarisTrackerToolNew::ReportInValidToolIdSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerToolNew::ReportInValidToolIdSpecifiedProcessing called ...\n");

  std::cerr << "Invalid tool id specified " << std::endl;
}
}
