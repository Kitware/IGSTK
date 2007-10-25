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

/** Constructor (initializes Aurora-specific tool values) */
AuroraTrackerToolNew::AuroraTrackerToolNew():m_StateMachine(this)
{
  //Tracker tool initialization flag
  m_TrackerToolInitialized = false;

  // SROM file flag
  m_SROMFileNameSpecified = false;

  // ToolId Specified
  m_ToolIdSpecified = false;

  // Channel number Specified
  m_ChannelNumberSpecified = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( PortNumberSpecified );
  igstkAddStateMacro( ChannelNumberSpecified );
  igstkAddStateMacro( SROMFileNameSpecified );
  igstkAddStateMacro( ToolIdSpecified );

  // Set the input descriptors
  igstkAddInputMacro( ValidPortNumber ); 
  igstkAddInputMacro( InValidPortNumber ); 
  igstkAddInputMacro( ValidChannelNumber ); 
  igstkAddInputMacro( InValidChannelNumber ); 
  igstkAddInputMacro( ValidSROMFileName ); 
  igstkAddInputMacro( InValidSROMFileName ); 
  igstkAddInputMacro( ValidToolId ); 
  igstkAddInputMacro( InValidToolId ); 

  m_StateMachine.SetReadyToRun();

  // Programming the state machine transitions:

  // Transitions from the Idle
  igstkAddTransitionMacro( Idle,
                           ValidPortNumber,
                           PortNumberSpecified,
                           SetPortNumber);

  igstkAddTransitionMacro( Idle,
                           InValidPortNumber,
                           Idle,
                           ReportInValidPortNumberSpecified );

  // Transitions from PortNumberSpecified
  igstkAddTransitionMacro( PortNumberSpecified,
                           ValidChannelNumber,
                           ChannelNumberSpecified,
                           SetChannelNumber);

  igstkAddTransitionMacro( PortNumberSpecified,
                           InValidChannelNumber,
                           PortNumberSpecified,
                           ReportInValidChannelNumberSpecified);

  // Transitions from ChannelNumberSpecified
  igstkAddTransitionMacro( ChannelNumberSpecified,
                           ValidSROMFileName,
                           SROMFileNameSpecified,
                           SetSROMFileName);

  igstkAddTransitionMacro( ChannelNumberSpecified,
                           InValidSROMFileName,
                           ChannelNumberSpecified,
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
AuroraTrackerToolNew::~AuroraTrackerToolNew()
{
}

/** Print Self function */
void AuroraTrackerToolNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Request the state machine to set the port number */
void AuroraTrackerToolNew::RequestSetPort( unsigned int portNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::AuroraTrackerToolNew::RequestSetPort called ...\n");

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

}
