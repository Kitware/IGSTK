/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    TrackerInitializer.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "TrackerInitializer.h"

/** Constructor: Initializes all internal variables. */
TrackerInitializer::TrackerInitializer( TrackerConfiguration * config)
{ 
    m_TrackerConfiguration = config;
    m_ErrorMessage  = "";
}

int TrackerInitializer::RequestInitializeTracker()
{
  if ( m_TrackerConfiguration == NULL )
  {
    m_ErrorMessage = "Invalid configuration file";
    return EXIT_FAILURE;
  }

  m_TrackerType = m_TrackerConfiguration->GetTrackerType();

  if ( m_TrackerType == TrackerConfiguration::Polaris )
    {
    m_ErrorMessage = "Initialization success";
    return EXIT_SUCCESS;
    }
  else if (m_TrackerType == TrackerConfiguration::Aurora)
    {
    m_ErrorMessage = "Initialization success";
    return EXIT_SUCCESS;
    }
  else if (m_TrackerType == TrackerConfiguration::Micron)
    {
    m_ErrorMessage = "Initialization success";
    return EXIT_SUCCESS;
    }
  else
    {
    m_ErrorMessage = "Invalid tracker type";
    return EXIT_FAILURE;
    }

}

int TrackerInitializer::InitializePolarisTracker()
{

  return EXIT_SUCCESS;

}

int TrackerInitializer::InitializeAuroraTracker()
{
  return EXIT_SUCCESS;

}

int TrackerInitializer::InitializeMicronTracker()
{
  return EXIT_SUCCESS;

}

/** Destructor */
TrackerInitializer::~TrackerInitializer()
{
  if (m_TrackerConfiguration)
  {
    delete m_TrackerConfiguration;
  }

}

