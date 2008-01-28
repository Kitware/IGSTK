/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerTool.cxx
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

#include "igstkAuroraTrackerTool.h"
#include <sstream>
#include <itksys/SystemTools.hxx>

namespace igstk
{

/** Constructor (configures Aurora-specific tool values) */
AuroraTrackerTool::AuroraTrackerTool():m_StateMachine(this)
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
AuroraTrackerTool::~AuroraTrackerTool()
{
}

/** Print Self function */
void AuroraTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Request to set 5DOF tracker tool*/
void AuroraTrackerTool::RequestSelect5DOFTrackerTool()
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::RequestSelect5DOFTrackerTool called ...\n");

  m_StateMachine.PushInput( m_Select5DOFTrackerToolInput ); 
  m_StateMachine.ProcessInputs();
}

/** Request to set 6DOF tracker tool*/
void AuroraTrackerTool::RequestSelect6DOFTrackerTool()
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::RequestSelect6DOFTrackerTool called ...\n");

  m_StateMachine.PushInput( m_Select6DOFTrackerToolInput ); 
  m_StateMachine.ProcessInputs();
}

/** Request the state machine to set the port number */
void AuroraTrackerTool::RequestSetPortNumber( unsigned int portNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::RequestSetPortNumber called ...\n");

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
void AuroraTrackerTool::RequestSetChannelNumber( unsigned int channelNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::RequestSetChannelNumber called ...\n");

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
void AuroraTrackerTool::RequestSetSROMFileName( std::string filename )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::RequestSetSROMFileName called ...\n");

  if ( filename == "" || !itksys::SystemTools::FileExists( filename.c_str() ) )
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
void AuroraTrackerTool::RequestSetToolId( std::string toolId )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::RequestSetToolId called ...\n");

  //FIXME DO ToolID verification 
  m_ToolIdToBeSet = toolId;
  m_StateMachine.PushInput( m_ValidToolIdInput );
  m_StateMachine.ProcessInputs();
}

/** Set valid port number */ 
void AuroraTrackerTool::SetPortNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::SetPortNumberProcessing called ...\n");
  m_PortNumber = m_PortNumberToBeSet;

  //Set the tracker tool unique identifier to the port number. If a
  //channel number is specified a unique identifier will be regenerated
  //using port and channel number information
  std::stringstream identifierStream;
  identifierStream << m_PortNumber;
  this->SetTrackerToolIdentifier( identifierStream.str() );

  // For 6DOF type tracker tool, once port number is specified, the
  // tracker tool can be marked as configured and can be attached to the
  // tracker. Specification of SROM file and tool ID is optional.
  if ( ! m_5DOFTrackerToolSelected ) 
    {
    m_TrackerToolConfigured = true;
    }
  // For 5DOF type tracker tool, channel number must be specified
}

/** Report 5DOF tracker tool selected */ 
void AuroraTrackerTool::Report5DOFTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::Report5DOFTrackerToolSelectedProcessing called ...\n");


  m_5DOFTrackerToolSelected = true;
}

/** Report 6DOF tracker tool selected */ 
void AuroraTrackerTool::Report6DOFTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::Report6DOFTrackerToolSelectedProcessing called ...\n");

  igstkLogMacro( DEBUG, "6DOF Tracker tool selected ");
}

/** Report Invalid port number specified */ 
void AuroraTrackerTool::ReportInValidPortNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::ReportInValidPortNumberSpecifiedProcessing called ...\n");

  igstkLogMacro( CRITICAL, "Invalid port numbner specified ");
}

/** Set valid channel number */ 
void AuroraTrackerTool::SetChannelNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::SetChannelNumberProcessing called ...\n");
  m_ChannelNumber = m_ChannelNumberToBeSet;
  m_ChannelNumberSpecified = true;

  std::stringstream identifierStream;
  identifierStream << m_PortNumber << "_" << m_ChannelNumber;
  this->SetTrackerToolIdentifier( identifierStream.str() );

  // for 5DOF type tracker tool, once port and channel number are specified, the
  // tracker tool can be marked as configured and can be attached to the
  // tracker. Specification of SROM file and tool ID is optional.
  m_TrackerToolConfigured = true;
}

/** Report Invalid channel number specified */ 
void AuroraTrackerTool::ReportInValidChannelNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::ReportInValidChannelNumberSpecifiedProcessing called ...\n");

  igstkLogMacro( CRITICAL, "Invalid Channel numbner specified ");
}

/** Set valid SROM filename */ 
void AuroraTrackerTool::SetSROMFileNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::SetSROMFileNameProcessing called ...\n");

  m_SROMFileName = m_SROMFileNameToBeSet;
  m_SROMFileNameSpecified = true;
}

/** Report Invalid SROM filename specified */ 
void AuroraTrackerTool::ReportInValidSROMFileSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::ReportInValidSROMFileSpecifiedProcessing called ...\n");

  igstkLogMacro( CRITICAL, "Invalid SROM file name specified ");
}

/** Set valid ToolId */ 
void AuroraTrackerTool::SetToolIdProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::SetToolIdProcessing called ...\n");

  m_ToolId = m_ToolIdToBeSet;
  m_ToolIdSpecified = true;
}

/** Report Invalid tool id  specified*/ 
void AuroraTrackerTool::ReportInValidToolIdSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::ReportInValidToolIdSpecifiedProcessing called ...\n");

  igstkLogMacro( CRITICAL, "Invalid ToolID specified ");
}

/** Check if the SROM filename is specified */
bool AuroraTrackerTool::IsSROMFileNameSpecified( )
{
  return m_SROMFileNameSpecified;
}

/** Get SROM filename */ 
std::string AuroraTrackerTool::GetSROMFileName( )
{
  return m_SROMFileName;
}

/** Get Port number */ 
unsigned int AuroraTrackerTool::GetPortNumber( )
{
  return m_PortNumber;
}

/** Check if the tracker tool is 5DOF or 6DOF */ 
bool AuroraTrackerTool::IsTrackerTool5DOF( )
{
  return m_5DOFTrackerToolSelected;
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker tool * is configured */ 
bool
AuroraTrackerTool::CheckIfTrackerToolIsConfigured( )
{
  igstkLogMacro( DEBUG, "igstk::AuroraTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

}
