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

#ifdef IGSTKSandbox_USE_MicronTracker
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#endif /* IGSTKSandbox_USE_MicronTracker */

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

  Tracker::Pointer                             m_Tracker;
  std::list< igstk::TrackerTool::Pointer >     m_TrackerToolList;

  PolarisTracker::Pointer               m_PolarisTracker;
  AuroraTracker::Pointer                m_AuroraTracker;
  
  int InitializePolarisTracker();
  int InitializeAuroraTracker();

#ifdef IGSTKSandbox_USE_MicronTracker
  MicronTracker::Pointer                m_MicronTracker;
  int InitializeMicronTracker();
#endif /* IGSTKSandbox_USE_MicronTracker */
  

  TrackerConfiguration * m_TrackerConfiguration;
  
  std::string            m_ErrorMessage;

};

} // end of namespace
#endif
