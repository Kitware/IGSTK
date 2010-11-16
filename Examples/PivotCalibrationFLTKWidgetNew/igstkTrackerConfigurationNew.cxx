/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerConfigurationNew.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerConfigurationNew.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
TrackerConfigurationNew::TrackerConfigurationNew()
{
  m_ValidityErrorMessage = "";
}

void TrackerConfigurationNew::SetTrackerType( TrackerType trackerType )
{
  m_TrackerType = trackerType;
  switch( m_TrackerType )
    {
    case Polaris:
    case Aurora:
      m_NDITrackerConfiguration = new NDITrackerConfiguration;
      break;
    case FlockOfBirds:
      m_AscensionTrackerConfiguration = new AscensionTrackerConfiguration;
      break;
    case Micron:
      m_MicronTrackerConfiguration = new MicronTrackerConfiguration;
      break;
    }
}

std::string TrackerConfigurationNew::GetTrackerTypeAsString()
{
  switch( m_TrackerType )
    {
    case Polaris:
      return "Polaris"; break;
    case Aurora:
      return "Aurora"; break;
    case FlockOfBirds:
      return "FlockOfBirds"; break;
    case Micron:
      return "Micron"; break;
    default:
      return "Not Defined";
    }
}
/** Destructor */
TrackerConfigurationNew::~TrackerConfigurationNew()
{
}

int TrackerConfigurationNew::CheckValidity()
{
  switch( m_TrackerType )
  {
  case Polaris:
    return CheckPolarisValidity();
    break;
  case Aurora:
    return CheckAuroraValidity();
    break;
  case FlockOfBirds:
    return CheckFlockOfBirdsValidity();
    break;
  case Micron:
    return CheckMicronValidity();
    break;
  default:
    m_ValidityErrorMessage = "Unknown tracker type";
    return 0;
  }
}

int TrackerConfigurationNew::CheckAuroraValidity()
{
  m_ValidityErrorMessage = "";

  if ( m_NDITrackerConfiguration->m_Frequency <= 0)
  {
    m_ValidityErrorMessage = "Invalid frequency, must be positive";
    return 0;
  }

  int n = m_NDITrackerConfiguration->m_TrackerToolList.size();
  int ReferenceToolcount = 0;
  for (unsigned int i=0; i<n; i++)
  {
    NDITrackerToolConfiguration * conf = 
                           m_NDITrackerConfiguration->m_TrackerToolList[i];

    // check if port is used, only one reference tool, SROM file exists 
    // To be implemented
    
  }
  return 1;
}

int TrackerConfigurationNew::CheckFlockOfBirdsValidity()
{
    m_ValidityErrorMessage = "";

    if ( m_AscensionTrackerConfiguration->m_Frequency <= 0)
    {
        m_ValidityErrorMessage = "Invalid frequency, must be positive";
        return 0;
    }
    int n = m_AscensionTrackerConfiguration->m_TrackerToolList.size();
    int ReferenceToolcount = 0;
    for (unsigned int i=0; i<n; i++)
    {
         AscensionTrackerToolConfiguration * conf = 
            m_AscensionTrackerConfiguration->m_TrackerToolList[i];

        // check if port is used, only one reference tool, SROM file exists 
        // To be implemented
    }
    return 1;
}

int TrackerConfigurationNew::CheckPolarisValidity()
{
  m_ValidityErrorMessage = "";
  // check if port is used, only one reference tool, SROM file exists 
  // To be implemented
  return 1;
}

int TrackerConfigurationNew::CheckMicronValidity()
{
  m_ValidityErrorMessage = "";
  // check calibration file, only one reference tool, template markers 
  // To be implemented
  return 1;
}

} // end of name space
