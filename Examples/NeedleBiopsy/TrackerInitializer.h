/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    TrackerInitializer.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __TrackerInitializer_h
#define __TrackerInitializer_h

#include "TrackerConfiguration.h"

#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkAuroraTracker.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"

namespace igstk
{


class TrackerInitializer
{

public:
  
  typedef TrackerConfiguration::TrackerType  TrackerType;

  TrackerInitializer( TrackerConfiguration * config );

  int RequestInitializeTracker();
  
  itkGetStringMacro( ErrorMessage );
  
  igstk::Tracker::Pointer GetTracker()
  {
    return m_Tracker;
  }

  std::list< igstk::TrackerTool::Pointer > GetToolList()
  {
    return m_TrackerToolList;
  }

  virtual ~TrackerInitializer();

private:
  
  TrackerType            m_TrackerType;

  igstk::Tracker::Pointer                      m_Tracker;
  std::list< igstk::TrackerTool::Pointer >     m_TrackerToolList;

  igstk::PolarisTracker::Pointer               m_PolarisTracker;
  igstk::AuroraTracker::Pointer                m_AuroraTracker;
  igstk::MicronTracker::Pointer                m_MicronTracker;

  int InitializePolarisTracker();
  int InitializeAuroraTracker();
  int InitializeMicronTracker();

  TrackerConfiguration * m_TrackerConfiguration;
  
  std::string            m_ErrorMessage;

};

} // end of namespace
#endif
