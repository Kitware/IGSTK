/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerTool2.cxx
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

#include "igstkPolarisTrackerTool2.h"
#include "igstkPolarisTracker2.h"
#include <sstream>

#include <itksys/SystemTools.hxx>

namespace igstk
{

/** Constructor (configures Polaris-specific tool values) */
PolarisTrackerTool2::PolarisTrackerTool2():m_StateMachine(this)
{

  //Tracker tool configuration flag
  m_TrackerToolConfigured = false;

  // SROM file flag
  m_SROMFileNameSpecified = false;

  // PartNumber Specified
  m_PartNumberSpecified = false;

  // Wireless tool selected
  m_WirelessTrackerToolSelected  = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( WirelessTrackerToolSelected );
  igstkAddStateMacro( WiredTrackerToolSelected );
  igstkAddStateMacro( WiredTrackerToolPortNumberSpecified );
  igstkAddStateMacro( WirelessTrackerToolSROMFileNameSpecified );
  igstkAddStateMacro( WiredTrackerToolSROMFileNameSpecified );
  igstkAddStateMacro( WirelessTrackerToolPartNumberSpecified );
  igstkAddStateMacro( WiredTrackerToolPartNumberSpecified );


  // Set the input descriptors
  igstkAddInputMacro( SelectWirelessTrackerTool );
  igstkAddInputMacro( SelectWiredTrackerTool); 
  igstkAddInputMacro( ValidPortNumber ); 
  igstkAddInputMacro( InValidPortNumber ); 
  igstkAddInputMacro( ValidSROMFileName ); 
  igstkAddInputMacro( InValidSROMFileName ); 
  igstkAddInputMacro( ValidPartNumber ); 
  igstkAddInputMacro( InValidPartNumber ); 


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
                           ValidPartNumber,
                           WiredTrackerToolPartNumberSpecified,
                           SetPartNumber);

  igstkAddTransitionMacro( WiredTrackerToolSROMFileNameSpecified,
                           InValidPartNumber,
                           WiredTrackerToolSROMFileNameSpecified,
                           ReportInValidPartNumberSpecified);

  // Transitions from WirelessTrackerToolSROMFileNameSpecified
  igstkAddTransitionMacro( WirelessTrackerToolSROMFileNameSpecified,
                           ValidPartNumber,
                           WirelessTrackerToolPartNumberSpecified,
                           SetPartNumber);

  igstkAddTransitionMacro( WirelessTrackerToolSROMFileNameSpecified,
                           InValidPartNumber,
                           WirelessTrackerToolSROMFileNameSpecified,
                           ReportInValidPartNumberSpecified);


  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();

}

/** Destructor */
PolarisTrackerTool2::~PolarisTrackerTool2()
{
}

/** Print Self function */
void PolarisTrackerTool2::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PortNumber: "   << this->m_PortNumber << std::endl;
  os << indent << "SROMFileName: " << this->m_SROMFileName << std::endl;
  
  if( this->m_PartNumberSpecified )
    {
    os << indent << "Part number: "  << this->m_PartNumber << std::endl;
    }
  if( this->m_WirelessTrackerToolSelected )
    {
    os << indent << " Tracker tool is wireless type"  << std::endl;
    }
  else
    {
    os << indent << " Tracker tool is wired/active type"  << std::endl;
    }
}

/** Request the state machine to select wired tracker tool model. */
void PolarisTrackerTool2::RequestSelectWiredTrackerTool( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::RequestSelectWiredTrackerTool called ...\n");

  m_StateMachine.PushInput( m_SelectWiredTrackerToolInput );
  m_StateMachine.ProcessInputs();
}

/** Request the state machine to select wireless tracker tool model. */
void PolarisTrackerTool2::RequestSelectWirelessTrackerTool( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::RequestSelectWirelessTrackerTool called ...\n");

  m_StateMachine.PushInput( m_SelectWirelessTrackerToolInput );
  m_StateMachine.ProcessInputs();
}
 
/** Request the state machine to set the port number.  */
void PolarisTrackerTool2::RequestSetPortNumber( unsigned int portNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::RequestSetPortNumber called ...\n");

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

/** Request the state machine to set the SROM file name.  */
void PolarisTrackerTool2::RequestSetSROMFileName( const std::string & filename )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::RequestSetSROMFileName called ...\n");
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

/** Check if the tool is wireless type. */
bool PolarisTrackerTool2::IsToolWirelessType( ) const
{
  return m_WirelessTrackerToolSelected;
}

/** Check if the SROM filename is specified.  */
bool PolarisTrackerTool2::IsSROMFileNameSpecified( ) const
{
  return m_SROMFileNameSpecified;
}

/** Check if Part number is specified.  */
bool PolarisTrackerTool2::IsPartNumberSpecified( ) const
{
  return m_PartNumberSpecified;
}


/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker
 * tool is configured. */ 
bool
PolarisTrackerTool2::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** Request the state machine to set the tool part number */
void PolarisTrackerTool2::RequestSetPartNumber( const std::string & partNumber )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::RequestSetPartNumber called ...\n");

  m_PartNumberToBeSet = partNumber;
  m_StateMachine.PushInput( m_ValidPartNumberInput );
  m_StateMachine.ProcessInputs();
}

/** Report wireless tracker tool selected */ 
void PolarisTrackerTool2::ReportWirelessTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::ReportWirelessTrackerToolSelected called ...\n");

  m_WirelessTrackerToolSelected = true;
}

/** Report wired tracker tool selected */ 
void PolarisTrackerTool2::ReportWiredTrackerToolSelectedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::ReportWiredTrackerToolSelected called ...\n");

}

/** Set valid port number */ 
void PolarisTrackerTool2::SetPortNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::SetPortNumberProcessing called ...\n");
  m_PortNumber = m_PortNumberToBeSet;
  
  std::stringstream identifierStream;
  identifierStream << m_PortNumber;
  
  // For polaris tracker, port number could be used as a unique identifier
  this->SetTrackerToolIdentifier( identifierStream.str() );

  // if the tool is wired type and if the port number is set, we can
  // "declare" the tracker tool configured. The user can also specify
  // SROM file and tool part number.  
  if ( ! m_WirelessTrackerToolSelected )
    {
    m_TrackerToolConfigured = true;
    }
}

/** Report Invalid port number specified */ 
void PolarisTrackerTool2::ReportInValidPortNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::ReportInValidPortNumberSpecifiedProcessing "
    "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid Port Number specified ");
}

/** Set valid SROM filename */ 
void PolarisTrackerTool2::SetSROMFileNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::SetSROMFileNameProcessing called ...\n");

  m_SROMFileName = m_SROMFileNameToBeSet;
  m_SROMFileNameSpecified = true;

  //At this point, SROM file is specified. For wireless
  //tracker tools this is the minimum requirement. The tool can be "declared"
  //intialized.
  if ( m_WirelessTrackerToolSelected )
    {
    m_TrackerToolConfigured = true;

    //unique identifer needs to be established 
    //use the root name of the SROM file
    std::string identifier = 
          itksys::SystemTools::GetFilenameWithoutExtension( m_SROMFileName );
  
    this->SetTrackerToolIdentifier( identifier );
    }
}

/** Report Invalid SROM filename specified */ 
void PolarisTrackerTool2::ReportInValidSROMFileSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::ReportInValidSROMFileSpecifiedProcessing "
    "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid SROM filename specified ");
}

/** Set valid PartNumber */ 
void PolarisTrackerTool2::SetPartNumberProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::SetPartNumberProcessing called ...\n");

  m_PartNumber = m_PartNumberToBeSet;
  m_PartNumberSpecified = true;
}

/** Report Invalid tool part number  specified. */
void PolarisTrackerTool2::ReportInValidPartNumberSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::PolarisTrackerTool2::ReportInValidPartNumberSpecifiedProcessing "
    "called ...\n");
  igstkLogMacro( CRITICAL, "Invalid tool ID specified" );
}

/** The "RequestAttachToTracker" method attaches 
 * the tracker tool to a tracker. */
void PolarisTrackerTool2::RequestAttachToTracker( PolarisTracker2 *  tracker )
{
  // This delegation is done only to enforce type matching between
  // TrackerTool and Tracker. It prevents the user from accidentally 
  // mix TrackerTools and Trackers of different type;
  this->TrackerTool::RequestAttachToTracker( tracker );
}

}
