/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    $$
Language:  C++
Date:      $$
Version:   $$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkTrackerInitializerNew_h
#define __igstkTrackerInitializerNew_h

#include "igstkTrackerConfigurationNew.h"

//#include "igstkSandboxConfigure.h"

#include "IGSTKErrorObserver.h"

#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkAuroraTracker.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkFlockOfBirdsTrackerNew.h"
#include "igstkFlockOfBirdsTrackerToolNew.h"

#ifdef IGSTKSandbox_USE_MicronTracker
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#endif /* IGSTKSandbox_USE_MicronTracker */

namespace igstk
{

/** \class TrackerInitializerNew
* 
* \brief A convenient class for setting up trackers.
*
* This class provides a generic API to talk to different trackers. 
* User needs to set the TrackerConfiguration first, after the 
* RequestInitializeTracker returns success. You can call
*     GetTracker()
*     GetNonReferenceToolList()
*     GetReferenceTool()
* to get pointers to the initialized tracker and tracker tool objects.
*/

class TrackerInitializerNew
{

public:
  
  typedef TrackerConfigurationNew::TrackerType  TrackerType;

  typedef igstk::Logger                         LoggerType;

  TrackerInitializerNew( );

  void SetLogger(LoggerType* logger)
  {
     m_Logger = logger;
  }

  TrackerType GetTrackerType()
    {
    return m_TrackerType;
    }

  std::string GetTrackerTypeAsString();

  void SetTrackerConfiguration( TrackerConfigurationNew * config )
    {
    m_TrackerConfiguration = config;
    }

  int RequestInitializeTracker();
  
  int RequestStopAndDisconnectTracker();
  
  itkGetStringMacro( ErrorMessage );
  
  igstk::Tracker::Pointer GetTracker()
    {
    return m_Tracker;
    }

  std::vector< TrackerTool::Pointer > GetNonReferenceToolList()
    {
    return m_TrackerToolList;
    }

  int HasReferenceTool()
    {
    return m_HasReferenceTool;
    }

  TrackerTool::Pointer GetReferenceTool()
    {
    return m_ReferenceTool;
    }

  void StopAndCloseTracker();

  virtual ~TrackerInitializerNew();

private:
  
  TrackerType                           m_TrackerType;

  Tracker::Pointer                      m_Tracker;
  std::vector< TrackerTool::Pointer >   m_TrackerToolList;
  bool                                  m_HasReferenceTool;
  TrackerTool::Pointer                  m_ReferenceTool;

  PolarisTracker::Pointer               m_PolarisTracker;
  AuroraTracker::Pointer                m_AuroraTracker;
  FlockOfBirdsTracker::Pointer          m_FlockOfBirdsTracker;

  SerialCommunication::Pointer          m_Communication;

  LoggerType                            *m_Logger;

  IGSTKErrorObserver::Pointer           m_ErrorObserver;
  
  int InitializePolarisTracker();
  int InitializeAuroraTracker();
  int InitializeFlockOfBirdsTracker();

#ifdef IGSTKSandbox_USE_MicronTracker
  MicronTracker::Pointer                m_MicronTracker;
  int InitializeMicronTracker();
#endif /* IGSTKSandbox_USE_MicronTracker */
  

  TrackerConfigurationNew *             m_TrackerConfiguration;
  
  std::string                           m_ErrorMessage;

};

} // end of namespace
#endif
