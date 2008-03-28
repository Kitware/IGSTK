/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQMouseTrackerTool.cxx
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

#include "igstkQMouseTrackerTool.h"
#include "igstkQMouseTracker.h"

namespace igstk
{

/** Constructor (configures Polaris-specific tool values) */
QMouseTrackerTool::QMouseTrackerTool():m_StateMachine(this)
{
  m_TrackerToolConfigured = false;
}

/** Destructor */
QMouseTrackerTool::~QMouseTrackerTool()
{
}

/** Request set mouse name */
void QMouseTrackerTool::RequestSetMouseName( const MouseNameType & mouseName )
{
  this->m_MouseName = mouseName;
  m_TrackerToolConfigured = true;

  // For MouseTracker, mouse name is used as a unique identifier
  this->SetTrackerToolIdentifier( m_MouseName );
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker
 * tool * is configured */
bool
QMouseTrackerTool::CheckIfTrackerToolIsConfigured( ) const
{
  igstkLogMacro( DEBUG,
   "igstk::PolarisTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** Print Self function */
void QMouseTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} //igstk namespace
