/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWebCameraTrackerTool.cxx
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

#include "igstkWebCameraTrackerTool.h"
#include "igstkWebCameraTracker.h"

namespace igstk
{

/** Constructor (configures Polaris-specific tool values) */
WebCameraTrackerTool::WebCameraTrackerTool():m_StateMachine(this)
{
  m_TrackerToolConfigured = false;
}

/** Destructor */
WebCameraTrackerTool::~WebCameraTrackerTool()
{
}

/** Request set name */
void WebCameraTrackerTool::RequestSetName( const NameType & name )
{
  this->m_Name = name;
  m_TrackerToolConfigured = true;

  // For Tracker, name is used as a unique identifier
  this->SetTrackerToolIdentifier( m_Name ); 
}

/** The "CheckIfTrackerToolIsConfigured" method returns true if the tracker tool * is configured */ 
bool
WebCameraTrackerTool::CheckIfTrackerToolIsConfigured( )
{
  igstkLogMacro( DEBUG, "igstk::PolarisTrackerTool::CheckIfTrackerToolIsConfigured called...\n");
  return m_TrackerToolConfigured;
}

/** The "RequestAttachToTracker" method attaches 
 * the tracker tool to a tracker. */
void WebCameraTrackerTool::RequestAttachToTracker( WebCameraTracker *  tracker )
{
  // This delegation is done only to enforce type matching between
  // TrackerTool and Tracker. It prevents the user from accidentally 
  // mix TrackerTools and Trackers of different type;
  this->TrackerTool::RequestAttachToTracker( tracker );
}

/** Print Self function */
void WebCameraTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} //igstk namespace
