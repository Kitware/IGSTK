/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTrackerTool.cxx
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

#include "igstkMouseTrackerTool.h"
#include "igstkMouseTracker.h"

namespace igstk
{

/** Constructor (configures Mouse-specific tool values) */
MouseTrackerTool::MouseTrackerTool():m_StateMachine(this)
{
  m_TrackerToolConfigured = false;
}

/** Destructor */
MouseTrackerTool::~MouseTrackerTool()
{
}

/** Request set mouse name */
void MouseTrackerTool::RequestSetMouseName( const MouseNameType & mouseName )
{
  this->m_MouseName = mouseName;
  m_TrackerToolConfigured = true;

  // For MouseTracker, mouse name is used as a unique identifier
  this->SetTrackerToolIdentifier( m_MouseName ); 
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if 
 *  the tracker tool * is configured */ 
bool
MouseTrackerTool::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG, 
     "igstk::MouseTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** Print Self function */
void MouseTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} //igstk namespace
