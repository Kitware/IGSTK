/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerToolNew.cxx
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

#include "igstkMicronTrackerToolNew.h"

namespace igstk
{

/** Constructor (initializes Polaris-specific tool values) */
MicronTrackerToolNew::MicronTrackerToolNew():m_StateMachine(this)
{
  m_TrackerToolInitialized = false;
}

/** Destructor */
MicronTrackerToolNew::~MicronTrackerToolNew()
{
}

/** Request set marker name */
void MicronTrackerToolNew::RequestSetMarkerName( std::string markerName )
{
  this->m_MarkerName = markerName;
  m_TrackerToolInitialized = true;

  // For MicronTracker, marker name is used as a unique identifier
  this->SetTrackerToolIdentifier( m_MarkerName ); 
}

/** The "GetTrackerToolInitialized" method returns true if the tracker tool * is initialized */ 
bool
MicronTrackerToolNew::GetTrackerToolInitialized( )
{
  igstkLogMacro( DEBUG, "igstk::PolarisTrackerToolNew::GetTrackerToolInitialized called...\n");
  return m_TrackerToolInitialized;
}


/** Print Self function */
void MicronTrackerToolNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} //igstk namespace
