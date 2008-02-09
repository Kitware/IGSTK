/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerTool2.cxx
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

#include "igstkAuroraTrackerTool2.h"
#include "igstkAuroraTracker2.h"
#include <sstream>
#include "itksys/SystemTools.hxx"

namespace igstk
{

/** Constructor (configures Aurora-specific tool values) */
AuroraTrackerTool2::AuroraTrackerTool2():m_StateMachine(this)
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
AuroraTrackerTool2::~AuroraTrackerTool2()
{
}

/** Print Self function */
void AuroraTrackerTool2::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Request to set 5DOF tracker tool. */
void AuroraTrackerTool2::RequestSelect5DOFTrackerTool()
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::RequestSelect5DOFTrackerTool called ...\n");

  m_StateMachine.PushInput( m_Select5DOFTrackerToolInput ); 
  m_StateMachine.ProcessInputs();
}

/** Request to set 6DOF tracker tool. */
void AuroraTrackerTool2::RequestSelect6DOFTrackerTool()
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::RequestSelect6DOFTrackerTool called ...\n");

  m_StateMachine.PushInput( m_Select6DOFTrackerToolInput ); 
  m_StateMachine.ProcessInputs();
}

/** Request the state machine to set the port number */
void AuroraTrackerTool2::RequestSetPortNumber( unsigned int portNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::RequestSetPortNumber called ...\n");

  const unsigned int MAXIMUM_PORT_NUMBER = 12;

  if ( portNumber > MAXIMUM_PORT_NUMBER )
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
void AuroraTrackerTool2::RequestSetChannelNumber( unsigned int channelNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::RequestSetChannelNumber called ...\n");

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
void AuroraTrackerTool2::RequestSetSROMFileName( const std::string & filename )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::RequestSetSROMFileName called ...\n");

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
void AuroraTrackerTool2::RequestSetPartNumber( const std::string & partNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::RequestSetPartNumber called ...\n");

  m_PartNumberToBeSet = partNumber;
  m_StateMachine.PushInput( m_ValidPartNumberInput );
  m_StateMachine.ProcessInputs();
}

/** Set valid port number */ 
void AuroraTrackerTool2::SetPortNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::SetPortNumberProcessing called ...\n");
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
void AuroraTrackerTool2::Report5DOFTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::Report5DOFTrackerToolSelectedProcessing "
    << "called ...\n");


  m_FiveDOFTrackerToolSelected = true;
}

/** Report 6DOF tracker tool selected */ 
void AuroraTrackerTool2::Report6DOFTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::Report6DOFTrackerToolSelectedProcessing "
    << "called ...\n");

  igstkLogMacro( DEBUG, "6DOF Tracker tool selected ");
}

/** Report Invalid port number specified */ 
void AuroraTrackerTool2::ReportInValidPortNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::ReportInValidPortNumberSpecifiedProcessing "
    << "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid port numbner specified ");
}

/** Set valid channel number */ 
void AuroraTrackerTool2::SetChannelNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::SetChannelNumberProcessing called ...\n");
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
void AuroraTrackerTool2::ReportInValidChannelNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::ReportInValidChannelNumberSpecifiedProcessing "
    << "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid Channel numbner specified ");
}

/** Set valid SROM filename */ 
void AuroraTrackerTool2::SetSROMFileNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::SetSROMFileNameProcessing called ...\n");

  m_SROMFileName = m_SROMFileNameToBeSet;
  m_SROMFileNameSpecified = true;
}

/** Report Invalid SROM filename specified */ 
void AuroraTrackerTool2::ReportInValidSROMFileSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::ReportInValidSROMFileSpecifiedProcessing "
    << "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid SROM file name specified ");
}

/** Set valid PartNumber */ 
void AuroraTrackerTool2::SetPartNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::SetPartNumberProcessing called ...\n");

  m_PartNumber = m_PartNumberToBeSet;
  m_PartNumberSpecified = true;
}

/** Report Invalid part number  specified. */ 
void AuroraTrackerTool2::ReportInValidPartNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::ReportInValidPartNumberSpecifiedProcessing "
    << "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid PartNumber specified ");
}

/** Check if the SROM filename is specified */
bool AuroraTrackerTool2::IsSROMFileNameSpecified( ) const
{
  return m_SROMFileNameSpecified;
}

/** Check if the Part number is specified */
bool AuroraTrackerTool2::IsPartNumberSpecified( ) const
{
  return m_PartNumberSpecified;
}

/** Check if the tracker tool is 5DOF or 6DOF */ 
bool AuroraTrackerTool2::IsTrackerTool5DOF( ) const
{
  return m_FiveDOFTrackerToolSelected;
}

/** The "RequestAttachToTracker" method attaches 
 * the tracker tool to a tracker. */
void AuroraTrackerTool2::RequestAttachToTracker( AuroraTracker2 *  tracker )
{
  // This delegation is done only to enforce type matching between
  // TrackerTool and Tracker. It prevents the user from accidentally 
  // mix TrackerTools and Trackers of different type;
  this->TrackerTool::RequestAttachToTracker( tracker );
}

/** The "CheckIfTrackerToolIsConfigured" method 
 * returns true if the tracker tool is configured */ 
bool
AuroraTrackerTool2::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerTool2::CheckIfTrackerToolIsConfigured "
    << "called...\n");
  return m_TrackerToolConfigured;
}

}
