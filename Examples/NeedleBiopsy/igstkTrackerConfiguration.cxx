/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerConfiguration.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerConfiguration.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
TrackerConfiguration::TrackerConfiguration()
{
  m_ValidityErrorMessage = "";
}

void TrackerConfiguration::SetTrackerType( TrackerType trackerType )
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
}

int TrackerConfiguration::CheckValidity()
{
  switch( m_TrackerType )
  {
  case Polaris:
    return CheckPolarisValidity();
    break;
  case Aurora:
    return CheckAuroraValidity();
    break;
  case Micron:
    return CheckMicronValidity();
    break;
  default:
    m_ValidityErrorMessage = "Unknown tracker type";
    return 0;
  }
}

int TrackerConfiguration::CheckAuroraValidity()
{
  m_ValidityErrorMessage = "";

  if ( m_NDITrackerConfiguration->m_Frequency <= 0)
  {
    m_ValidityErrorMessage = "Invalid frequency, must be positive";
    return 0;
  }

  unsigned int n = m_NDITrackerConfiguration->m_TrackerToolList.size();
  for (unsigned int i=0; i<n; i++)
  {
    //NDITrackerToolConfiguration * conf = 
    //                       m_NDITrackerConfiguration->m_TrackerToolList[i];

    // check if port is used, only one reference tool, SROM file exists 
    // To be implemented
    
  }
  return 1;
}

int TrackerConfiguration::CheckPolarisValidity()
{
  m_ValidityErrorMessage = "";
  // check if port is used, only one reference tool, SROM file exists 
  // To be implemented
  return 1;

}

int TrackerConfiguration::CheckMicronValidity()
{
  m_ValidityErrorMessage = "";
  // check calibration file, only one reference tool, template markers 
  // To be implemented
  return 1;
}

} // end of name space
