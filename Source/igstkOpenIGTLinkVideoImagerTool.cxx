/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkOpenIGTLinkVideoImagerTool.cxx
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

#include "igstkOpenIGTLinkVideoImagerTool.h"
#include "igstkOpenIGTLinkVideoImager.h"

namespace igstk
{

/** Constructor (configures OpenIGTlink-specific tool values) */
OpenIGTLinkVideoImagerTool::OpenIGTLinkVideoImagerTool():m_StateMachine(this)
{
  m_VideoImagerToolConfigured = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( VideoImagerToolNameSpecified );

  // Set the input descriptors
  igstkAddInputMacro( ValidVideoImagerToolName );
  igstkAddInputMacro( InValidVideoImagerToolName );


  // Add transitions
  //
  // Transitions from idle state
  igstkAddTransitionMacro( Idle,
                           ValidVideoImagerToolName,
                           VideoImagerToolNameSpecified,
                           SetVideoImagerToolName );

  igstkAddTransitionMacro( Idle,
                           InValidVideoImagerToolName,
                           Idle,
                           ReportInvalidVideoImagerToolNameSpecified );

  // Transitions from VideoImagerToolName specified
  igstkAddTransitionMacro( VideoImagerToolNameSpecified,
                           ValidVideoImagerToolName,
                           VideoImagerToolNameSpecified,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( VideoImagerToolNameSpecified,
                           InValidVideoImagerToolName,
                           VideoImagerToolNameSpecified,
                           ReportInvalidRequest );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();
}

/** Destructor */
OpenIGTLinkVideoImagerTool::~OpenIGTLinkVideoImagerTool()
{
}

/** Request set VideoImagerTool name */
void OpenIGTLinkVideoImagerTool
::RequestSetVideoImagerToolName( const std::string& clientDeviceName )
{
  igstkLogMacro( DEBUG, "igstk::OpenIGTLinkVideoImagerTool"
                             << "::RequestSetVideoImagerToolName called ...\n");
  if ( clientDeviceName == "" )
    {
    m_StateMachine.PushInput( m_InValidVideoImagerToolNameInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_VideoImagerToolNameToBeSet = clientDeviceName;
    m_StateMachine.PushInput( m_ValidVideoImagerToolNameInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set valid VideoImagerTool name */
void OpenIGTLinkVideoImagerTool::SetVideoImagerToolNameProcessing( )
{
  igstkLogMacro( DEBUG, "igstk::OpenIGTLinkVideoImagerTool"
                          << "::SetVideoImagerToolNameProcessing called ...\n");

  this->m_VideoImagerToolName = m_VideoImagerToolNameToBeSet;

  m_VideoImagerToolConfigured = true;

  // VideoImagerTool name is used as a unique identifier
  this->SetVideoImagerToolIdentifier( this->m_VideoImagerToolName );
}

/** Report Invalid VideoImagerTool name*/
void OpenIGTLinkVideoImagerTool
::ReportInvalidVideoImagerToolNameSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, "igstk::OpenIGTLinkVideoImagerTool"
       << "::ReportInvalidVideoImagerToolNameSpecifiedProcessing called ...\n");

  igstkLogMacro( CRITICAL, "Invalid VideoImagerTool name specified ");
}

void OpenIGTLinkVideoImagerTool::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** The "CheckIfVideoImagerToolIsConfigured" method returns true if the tracker
 * tool is configured */
bool
OpenIGTLinkVideoImagerTool::CheckIfVideoImagerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, "igstk::OpenIGTLinkVideoImagerTool"
                         << "::CheckIfVideoImagerToolIsConfigured called...\n");
  return m_VideoImagerToolConfigured;
}

/** Print Self function */
void OpenIGTLinkVideoImagerTool
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "VideoImagerTool name : "
    << m_VideoImagerToolName << std::endl;
  os << indent << "VideoImagerToolConfigured:"
    << m_VideoImagerToolConfigured << std::endl;
}
}
