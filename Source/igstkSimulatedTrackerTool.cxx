/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSimulatedTrackerTool.cxx
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

#include "igstkSimulatedTrackerTool.h"
#include "igstkSimulatedTracker.h"

namespace igstk
{

/** Constructor (configures Polaris-specific tool values) */
SimulatedTrackerTool::SimulatedTrackerTool():m_StateMachine(this)
{
  m_TrackerToolConfigured = false;
}

/** Destructor */
SimulatedTrackerTool::~SimulatedTrackerTool()
{
}

/** Request set name */
void SimulatedTrackerTool::RequestSetName( const NameType & name )
{
  this->m_Name = name;
  m_TrackerToolConfigured = true;

  // For Tracker, name is used as a unique identifier
  this->SetTrackerToolIdentifier( m_Name ); 
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if 
 * the tracker tool * is configured */ 
bool
SimulatedTrackerTool::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
      "igstk::PolarisTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** Print Self function */
void 
SimulatedTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} //igstk namespace
