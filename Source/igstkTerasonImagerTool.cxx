/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTerasonImagerTool.cxx
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

#include "igstkTerasonImagerTool.h"
#include "igstkTerasonImager.h"

namespace igstk
{

/** Constructor (configures OpenIGTlink-specific tool values) */
TerasonImagerTool::TerasonImagerTool():m_StateMachine(this)
{
  m_ImagerToolConfigured = false;

  // States
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ImagerToolNameSpecified );

  // Set the input descriptors
  igstkAddInputMacro( ValidImagerToolName );
  igstkAddInputMacro( InValidImagerToolName );


  // Add transitions
  //
  // Transitions from idle state
  igstkAddTransitionMacro( Idle,
                           ValidImagerToolName,
                           ImagerToolNameSpecified,
                           SetImagerToolName );

  igstkAddTransitionMacro( Idle,
                           InValidImagerToolName,
                           Idle,
                           ReportInvalidImagerToolNameSpecified );

  // Transitions from ImagerToolName specified
  igstkAddTransitionMacro( ImagerToolNameSpecified,
                           ValidImagerToolName,
                           ImagerToolNameSpecified,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( ImagerToolNameSpecified,
                           InValidImagerToolName,
                           ImagerToolNameSpecified,
                           ReportInvalidRequest );

  // Inputs to the state machine
  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();


}

/** Destructor */
TerasonImagerTool::~TerasonImagerTool()
{
}

/** Request set ImagerTool name */
void TerasonImagerTool::RequestSetImagerToolName( const std::string& clientDeviceName )
{
  igstkLogMacro( DEBUG, 
    "igstk::TerasonImagerTool::RequestSetImagerToolName called ...\n");
  if ( clientDeviceName == "" )
    {
    m_StateMachine.PushInput( m_InValidImagerToolNameInput );
    m_StateMachine.ProcessInputs();
    }
  else 
    {
    m_ImagerToolNameToBeSet = clientDeviceName;
    m_StateMachine.PushInput( m_ValidImagerToolNameInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set valid ImagerTool name */ 
void TerasonImagerTool::SetImagerToolNameProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TerasonImagerTool::SetImagerToolNameProcessing called ...\n");

  this->m_ImagerToolName = m_ImagerToolNameToBeSet;

  m_ImagerToolConfigured = true;

  // ImagerTool name is used as a unique identifier
  this->SetImagerToolIdentifier( this->m_ImagerToolName );

  std::cout << " SetImagerToolIdentifier: " << this->m_ImagerToolName << std::endl;
}

/** Report Invalid ImagerTool name*/ 
void TerasonImagerTool::ReportInvalidImagerToolNameSpecifiedProcessing( )
{
  igstkLogMacro( DEBUG, 
    "igstk::TerasonImagerTool::ReportInvalidImagerToolNameSpecifiedProcessing "
    "called ...\n");

  igstkLogMacro( CRITICAL, "Invalid ImagerTool name specified ");
}

void TerasonImagerTool::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** The "CheckIfImagerToolIsConfigured" method returns true if the tracker
 * tool is configured */ 
bool
TerasonImagerTool::CheckIfImagerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::TerasonImagerTool::CheckIfImagerToolIsConfigured called...\n");
  return m_ImagerToolConfigured;
}

/** Print Self function */
void TerasonImagerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ImagerTool name : "    << m_ImagerToolName << std::endl;
  os << indent << "ImagerToolConfigured:" << m_ImagerToolConfigured << std::endl;
}
}
