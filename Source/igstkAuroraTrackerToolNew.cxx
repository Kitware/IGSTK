/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerToolNew.cxx
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
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkAuroraTrackerToolNew.h"
#include <sstream>

namespace igstk
{

/** Constructor (configures Aurora-specific tool values) */
AuroraTrackerToolNew::AuroraTrackerToolNew():m_StateMachine(this)
{
  //Tracker tool initialization flag
  m_TrackerToolConfigured = false;

  //Disable 5DOF tracker tool mode
  m_5DOFTrackerToolSelected = false;

  // SROM file flag
  m_SROMFileNameSpecified = false;

  // ToolId Specified
  m_ToolIdSpecified = false;

  // Channel number Specified
  m_ChannelNumberSpecified = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( 5DOFTrackerToolSelected );
  igstkAddStateMacro( 6DOFTrackerToolSelected );
  igstkAddStateMacro( 5DOFTrackerToolPortNumberSpecified );
  igstkAddStateMacro( 6DOFTrackerToolPortNumberSpecified );
  igstkAddStateMacro( 5DOFTrackerToolChannelNumberSpecified );
  igstkAddStateMacro( 5DOFTrackerToolSROMFileNameSpecified );
  igstkAddStateMacro( 6DOFTrackerToolSROMFileNameSpecified );
  igstkAddStateMacro( 5DOFTrackerToolToolIdSpecified );
  igstkAddStateMacro( 6DOFTrackerToolToolIdSpecified );

  // Set the input descriptors
  igstkAddInputMacro( Select5DOFTrackerTool );
  igstkAddInputMacro( Select6DOFTrackerTool );
  igstkAddInputMacro( ValidPortNumber ); 
  igstkAddInputMacro( InValidPortNumber ); 
  igstkAddInputMacro( ValidChannelNumber ); 
  igstkAddInputMacro( InValidChannelNumber ); 
  igstkAddInputMacro( ValidSROMFileName ); 
  igstkAddInputMacro( InValidSROMFileName ); 
  igstkAddInputMacro( ValidToolId ); 
  igstkAddInputMacro( InValidToolId ); 

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           Select5DOFTrackerTool,
                           5DOFTrackerToolSelected,
                           Report5DOFTrackerToolSelected);
  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           Select6DOFTrackerTool,
                           6DOFTrackerToolSelected,
                           Report6DOFTrackerToolSelected);


  // Transitions from 5DOFTrackerToolSelected 
  igstkAddTransitionMacro( 5DOFTrackerToolSelected,
                           ValidPortNumber,
                           5DOFTrackerToolPortNumberSpecified,
                           SetPortNumber);

  igstkAddTransitionMacro( 5DOFTrackerToolSelected,
                           InValidPortNumber,
                           5DOFTrackerToolSelected,
                           ReportInValidPortNumberSpecified );

  // Transitions from 6DOFTrackerToolSelected 
  igstkAddTransitionMacro( 6DOFTrackerToolSelected,
                           ValidPortNumber,
                           6DOFTrackerToolPortNumberSpecified,
                           SetPortNumber);

  igstkAddTransitionMacro( 6DOFTrackerToolSelected,
                           InValidPortNumber,
                           6DOFTrackerToolSelected,
                           ReportInValidPortNumberSpecified );


  // Transitions from 5DOFTrackerToolPortNumberSpecified
  igstkAddTransitionMacro( 5DOFTrackerToolPortNumberSpecified,
                           ValidChannelNumber,
                           5DOFTrackerToolChannelNumberSpecified,
                           SetChannelNumber);

  igstkAddTransitionMacro( 5DOFTrackerToolPortNumberSpecified,
                           InValidChannelNumber,
                           5DOFTrackerToolPortNumberSpecified,
                           ReportInValidChannelNumberSpecified);

  // Transitions from 5DOFTrackerToolChannelNumberSpecified
  igstkAddTransitionMacro( 5DOFTrackerToolChannelNumberSpecified,
                           ValidSROMFileName,
                           5DOFTrackerToolSROMFileNameSpecified,
                           SetSROMFileName);

  igstkAddTransitionMacro( 5DOFTrackerToolChannelNumberSpecified,
                           InValidSROMFileName,
                           5DOFTrackerToolChannelNumberSpecified,
                           ReportInValidSROMFileSpecified);

  // Transitions from 6DOFTrackerToolPortNumberSpecified 
  igstkAddTransitionMacro( 6DOFTrackerToolPortNumberSpecified,
                           ValidSROMFileName,
                           6DOFTrackerToolSROMFileNameSpecified,
                           SetSROMFileName);

  igstkAddTransitionMacro( 6DOFTrackerToolPortNumberSpecified,
                           InValidSROMFileName,
                           6DOFTrackerToolPortNumberSpecified,
                           ReportInValidSROMFileSpecified);


  // Transitions from 5DOFTrackerToolSROMFileNameSpecified
  igstkAddTransitionMacro( 5DOFTrackerToolSROMFileNameSpecified,
                           ValidToolId,
                           5DOFTrackerToolToolIdSpecified,
                           SetToolId);

  igstkAddTransitionMacro( 5DOFTrackerToolSROMFileNameSpecified,
                           InValidToolId,
                           5DOFTrackerToolSROMFileNameSpecified,
                           ReportInValidToolIdSpecified);

  // Transitions from SROMFileNameSpecified
  igstkAddTransitionMacro( 6DOFTrackerToolSROMFileNameSpecified,
                           ValidToolId,
                           6DOFTrackerToolToolIdSpecified,
                           SetToolId);

  igstkAddTransitionMacro( 6DOFTrackerToolSROMFileNameSpecified,
                           InValidToolId,
                           6DOFTrackerToolSROMFileNameSpecified,
                           ReportInValidToolIdSpecified);


  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );
  m_StateMachine.SetReadyToRun();

}

/** Destructor */
AuroraTrackerToolNew::~AuroraTrackerToolNew()
{
}

/** Print Self function */
void AuroraTrackerToolNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Request to set 5DOF tracker tool*/
void AuroraTrackerToolNew::RequestSelect5DOFTrackerTool()
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::RequestSelect5DOFTrackerTool called ...\n");

  m_StateMachine.PushInput( m_Select5DOFTrackerToolInput ); 
  m_StateMachine.ProcessInputs();
}

/** Request to set 6DOF tracker tool*/
void AuroraTrackerToolNew::RequestSelect6DOFTrackerTool()
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::RequestSelect6DOFTrackerTool called ...\n");

  m_StateMachine.PushInput( m_Select6DOFTrackerToolInput ); 
  m_StateMachine.ProcessInputs();
}

/** Request the state machine to set the port number */
void AuroraTrackerToolNew::RequestSetPortNumber( unsigned int portNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::RequestSetPortNumber called ...\n");

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

/** Request the state machine to set the channel number */
void AuroraTrackerToolNew::RequestSetChannelNumber( unsigned int channelNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::RequestSetChannelNumber called ...\n");

  if ( channelNumber > 1 )
    {
    m_StateMachine.PushInput( m_InValidChannelNumberInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    this->m_ChannelNumberToBeSet = channelNumber;
    m_StateMachine.PushInput( m_ValidChannelNumberInput );
    m_StateMachine.ProcessInputs();
    }
}



/** Request the state machine to set the SROM file name */
void AuroraTrackerToolNew::RequestSetSROMFileName( std::string filename )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::RequestSetSROMFileName called ...\n");

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
void AuroraTrackerToolNew::RequestSetToolId( std::string toolId )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::RequestSetToolId called ...\n");

  //FIXME DO ToolID verification 
  m_ToolIdToBeSet = toolId;
  m_StateMachine.PushInput( m_ValidToolIdInput );
  m_StateMachine.ProcessInputs();
}

/** Set valid port number */ 
void AuroraTrackerToolNew::SetPortNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::SetPortNumberProcessing called ...\n");
  m_PortNumber = m_PortNumberToBeSet;

  //Set the tracker tool unique identifier to the port number. If a
  //channel number is specified a unique identifier will be regenerated
  //using port and channel number information
  std::stringstream identifierStream;
  identifierStream << m_PortNumber;
  this->SetTrackerToolIdentifier( identifierStream.str() );
}

/** Report 5DOF tracker tool selected */ 
void AuroraTrackerToolNew::Report5DOFTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::Report5DOFTrackerToolSelectedProcessing called ...\n");

  std::cout << "5DOF Tracker tool selected " << std::endl;

  m_5DOFTrackerToolSelected = true;
}

/** Report 6DOF tracker tool selected */ 
void AuroraTrackerToolNew::Report6DOFTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::Report6DOFTrackerToolSelectedProcessing called ...\n");

  std::cout << "6DOF Tracker tool selected " << std::endl;
}

/** Report Invalid port number specified */ 
void AuroraTrackerToolNew::ReportInValidPortNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::ReportInValidPortNumberSpecifiedProcessing called ...\n");

  std::cerr << "Invalid port number specified " << std::endl;
}

/** Set valid channel number */ 
void AuroraTrackerToolNew::SetChannelNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::SetChannelNumberProcessing called ...\n");
  m_ChannelNumber = m_ChannelNumberToBeSet;
  m_ChannelNumberSpecified = true;

  //FIXME: Generate a unique identifier using the port and channel number
  //
  std::stringstream identifierStream;
  identifierStream << m_PortNumber << "_" << m_ChannelNumber;
  this->SetTrackerToolIdentifier( identifierStream.str() );
}

/** Report Invalid channel number specified */ 
void AuroraTrackerToolNew::ReportInValidChannelNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::ReportInValidChannelNumberSpecifiedProcessing called ...\n");
  std::cerr << "Invalid channel number specified " << std::endl;

}



/** Set valid SROM filename */ 
void AuroraTrackerToolNew::SetSROMFileNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::SetSROMFileNameProcessing called ...\n");

  m_SROMFileName = m_SROMFileNameToBeSet;
  m_SROMFileNameSpecified = true;
}

/** Report Invalid SROM filename specified */ 
void AuroraTrackerToolNew::ReportInValidSROMFileSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::ReportInValidSROMFileSpecifiedProcessing called ...\n");

  std::cerr << "Invalid  SROM file specified " << std::endl;
}

/** Set valid ToolId */ 
void AuroraTrackerToolNew::SetToolIdProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::SetToolIdProcessing called ...\n");

  m_ToolId = m_ToolIdToBeSet;
  m_ToolIdSpecified = true;
}

/** Report Invalid tool id  specified*/ 
void AuroraTrackerToolNew::ReportInValidToolIdSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::ReportInValidToolIdSpecifiedProcessing called ...\n");

  std::cerr << "Invalid  Tool id specified " << std::endl;
}

/** Check if the SROM filename is specified */
bool AuroraTrackerToolNew::IsSROMFileNameSpecified( )
{
  return m_SROMFileNameSpecified;
}

/** Get SROM filename */ 
std::string AuroraTrackerToolNew::GetSROMFileName( )
{
  return m_SROMFileName;
}

/** Get Port number */ 
unsigned int AuroraTrackerToolNew::GetPortNumber( )
{
  return m_PortNumber;
}

/** Check if the tracker tool is 5DOF or 6DOF */ 
bool AuroraTrackerToolNew::IsTrackerTool5DOF( )
{
  return m_5DOFTrackerToolSelected;
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker tool * is configured */ 
bool
AuroraTrackerToolNew::CheckIfTrackerToolIsConfigured( )
{
  igstkLogMacro( DEBUG, "igstk::AuroraTrackerToolNew::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

}
