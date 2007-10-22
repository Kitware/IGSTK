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
}

/** Destructor */
MicronTrackerToolNew::~MicronTrackerToolNew()
{
}

/** Get marker name */
std::string MicronTrackerToolNew::GetMarkerName( )
{
  return m_MarkerName;
}

/** Request set marker name */
void MicronTrackerToolNew::RequestSetMarkerName( std::string markerName )
{
  //FIXME: should be routed through the state machine
  this->m_MarkerName = markerName;
}

/** Print Self function */
void MicronTrackerToolNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} //igstk namespace
