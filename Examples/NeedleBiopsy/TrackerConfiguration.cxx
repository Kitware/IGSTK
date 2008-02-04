/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    TrackerConfiguration.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "TrackerConfiguration.h"

/** Constructor: Initializes all internal variables. */
TrackerConfiguration::TrackerConfiguration( TrackerType trackerType)
{
  m_TrackerType = trackerType;
  switch( m_TrackerType )
  {
  case Polaris:
  case Aurora:
    m_NDITrackerConfiguration = new NDITrackerConfiguration;
    break;
  case Micron:
    m_MicronTrackerConfiguration = new MicronTrackerConfiguration;
      break;
  }
    
}

std::string TrackerConfiguration::GetTrackerTypeAsString()
{
  switch( m_TrackerType )
  {
  case Polaris:
    return "Polaris"; break;
  case Aurora:
    return "Aurora"; break;
  case Micron:
    return "Micron"; break;
  default:
    return "Not Defined";
  }

}
/** Destructor */
TrackerConfiguration::~TrackerConfiguration()
{

  if (m_NDITrackerConfiguration)
  {
    delete m_NDITrackerConfiguration;
  }

  if (m_MicronTrackerConfiguration)
  {
    delete m_MicronTrackerConfiguration;
  }

}

