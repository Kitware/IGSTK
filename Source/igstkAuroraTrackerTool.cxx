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
#include "igstkAuroraTracker.h"
#include <sstream>
#include "itksys/SystemTools.hxx"

namespace igstk
{

/** Constructor (configures Aurora-specific tool values) */
AuroraTrackerTool::AuroraTrackerTool():m_StateMachine(this)
{
  //Tracker tool initialization flag
  m_TrackerToolConfigured = false;

  //Disable 5DOF tracker tool mode
  m_FiveDOFTrackerToolSelected = false;

  // SROM file flag
  m_SROMFileNameSpecified = false;

  // PartNumber Specified
  m_PartNumberSpecified = false;

  // Channel number Specified
  m_ChannelNumberSpecified = false;

  // Initialize internal variables accessable through GetMacros
  m_PortNumber    = 0;
  m_ChannelNumber = 0;
  m_SROMFileName  = "";
  m_PartNumber    = "";

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( 5DOFTrackerToolSelected );
  igstkAddStateMacro( 6DOFTrackerToolSelected );
  igstkAddStateMacro( 5DOFTrackerToolPortNumberSpecified );
  igstkAddStateMacro( 6DOFTrackerToolPortNumberSpecified );
  igstkAddStateMacro( 5DOFTrackerToolChannelNumberSpecified );
  igstkAddStateMacro( 5DOFTrackerToolSROMFileNameSpecified );
  igstkAddStateMacro( 6DOFTrackerToolSROMFileNameSpecified );
  igstkAddStateMacro( 5DOFTrackerToolPartNumberSpecified );
  igstkAddStateMacro( 6DOFTrackerToolPartNumberSpecified );

  // Set the input descriptors
  igstkAddInputMacro( Select5DOFTrackerTool );
  igstkAddInputMacro( Select6DOFTrackerTool );
  igstkAddInputMacro( ValidPortNumber ); 
  igstkAddInputMacro( InValidPortNumber ); 
  igstkAddInputMacro( ValidChannelNumber ); 
  igstkAddInputMacro( InValidChannelNumber ); 
  igstkAddInputMacro( ValidSROMFileName ); 
  igstkAddInputMacro( InValidSROMFileName ); 
  igstkAddInputMacro( ValidPartNumber ); 
  igstkAddInputMacro( InValidPartNumber ); 

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
                           ValidPartNumber,
                           5DOFTrackerToolPartNumberSpecified,
                           SetPartNumber);

  igstkAddTransitionMacro( 5DOFTrackerToolSROMFileNameSpecified,
                           InValidPartNumber,
                           5DOFTrackerToolSROMFileNameSpecified,
                           ReportInValidPartNumberSpecified);

  // Transitions from SROMFileNameSpecified
  igstkAddTransitionMacro( 6DOFTrackerToolSROMFileNameSpecified,
                           ValidPartNumber,
                           6DOFTrackerToolPartNumberSpecified,
                           SetPartNumber);

  igstkAddTransitionMacro( 6DOFTrackerToolSROMFileNameSpecified,
                           InValidPartNumber,
                           6DOFTrackerToolSROMFileNameSpecified,
                           ReportInValidPartNumberSpecified);


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

  os << indent << "PortNumber: "   << this->m_PortNumber << std::endl;
  os << indent << "SROMFileName: " << this->m_SROMFileName << std::endl;
  
  if( this->m_PartNumberSpecified )
    {
    os << indent << "Part number: "  << this->m_PartNumber << std::endl;
    }

  if( this->IsTrackerTool5DOF() )
    {
    os << indent << "Channel number" << m_ChannelNumber << std::endl;
    }
}

/** Request to set 5DOF tracker tool. */
void AuroraTrackerTool::RequestSelect5DOFTrackerTool()
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::RequestSelect5DOFTrackerTool called ...\n");

  m_StateMachine.PushInput( m_Select5DOFTrackerToolInput ); 
  m_StateMachine.ProcessInputs();
}

/** Request to set 6DOF tracker tool. */
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

  const unsigned int MAXIMUM_PORT_NUMBER = 4;

  if ( portNumber >= MAXIMUM_PORT_NUMBER )
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

  const unsigned int MAXIMUM_CHANNEL_NUMBER = 1;

  if ( channelNumber > MAXIMUM_CHANNEL_NUMBER )
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
void AuroraTrackerTool::RequestSetSROMFileName( const std::string & filename )
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

/** Request the state machine to set the part number */
void AuroraTrackerTool::RequestSetPartNumber( const std::string & partNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::RequestSetPartNumber called ...\n");

  if ( partNumber != "" )
    { 
    m_PartNumberToBeSet = partNumber;
    m_StateMachine.PushInput( m_ValidPartNumberInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_InValidPartNumberInput );
    m_StateMachine.ProcessInputs();
    }
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
  if ( ! m_FiveDOFTrackerToolSelected ) 
    {
    m_TrackerToolConfigured = true;
    }
  // For 5DOF type tracker tool, channel number must be specified
}

/** Report 5DOF tracker tool selected */ 
void AuroraTrackerTool::Report5DOFTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::Report5DOFTrackerToolSelectedProcessing "
    << "called ...\n");


  m_FiveDOFTrackerToolSelected = true;
}

/** Report 6DOF tracker tool selected */ 
void AuroraTrackerTool::Report6DOFTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::Report6DOFTrackerToolSelectedProcessing "
    << "called ...\n");

  igstkLogMacro( DEBUG, "6DOF Tracker tool selected ");
}

/** Report Invalid port number specified */ 
void AuroraTrackerTool::ReportInValidPortNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::ReportInValidPortNumberSpecifiedProcessing "
    << "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid port numbner specified ");

  this->InvokeEvent( InvalidAuroraPortNumberErrorEvent() );
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

  // For 5DOF type tracker tool, once port and channel number are specified, the
  // tracker tool can be marked as configured and can be attached to the
  // tracker. Specification of SROM file and tool ID is optional.
  m_TrackerToolConfigured = true;
}

/** Report Invalid channel number specified */ 
void AuroraTrackerTool::ReportInValidChannelNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::ReportInValidChannelNumberSpecifiedProcessing "
    << "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid Channel numbner specified ");

  this->InvokeEvent( InvalidAuroraChannelNumberErrorEvent() );
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
    "igstk::AuroraTrackerTool::ReportInValidSROMFileSpecifiedProcessing "
    << "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid SROM file name specified ");

  this->InvokeEvent( InvalidAuroraSROMFilenameErrorEvent() );
}

/** Set valid PartNumber */ 
void AuroraTrackerTool::SetPartNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::SetPartNumberProcessing called ...\n");

  m_PartNumber = m_PartNumberToBeSet;
  m_PartNumberSpecified = true;
}

/** Report Invalid part number  specified. */ 
void AuroraTrackerTool::ReportInValidPartNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::ReportInValidPartNumberSpecifiedProcessing "
    << "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid PartNumber specified ");

  this->InvokeEvent( InvalidAuroraPartNumberErrorEvent() );
}

/** Check if the SROM filename is specified */
bool AuroraTrackerTool::IsSROMFileNameSpecified( ) const
{
  return m_SROMFileNameSpecified;
}

/** Check if the Part number is specified */
bool AuroraTrackerTool::IsPartNumberSpecified( ) const
{
  return m_PartNumberSpecified;
}

/** Check if the tracker tool is 5DOF or 6DOF */ 
bool AuroraTrackerTool::IsTrackerTool5DOF( ) const
{
  return m_FiveDOFTrackerToolSelected;
}

/** The "CheckIfTrackerToolIsConfigured" method 
 * returns true if the tracker tool is configured */ 
bool
AuroraTrackerTool::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool::CheckIfTrackerToolIsConfigured "
    << "called...\n");
  return m_TrackerToolConfigured;
}

}
