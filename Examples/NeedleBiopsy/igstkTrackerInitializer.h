/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerInitializer.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkTrackerInitializer_h
#define __igstkTrackerInitializer_h

#include "igstkConfigure.h"

#include "igstkTrackerConfiguration.h"

#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkAuroraTracker.h"
#include "igstkAuroraTrackerTool.h"

#ifdef IGSTK_USE_MicronTracker
#include "igstkMicronTracker.h"
#include "igstkMicronTrackerTool.h"
#endif /* IGSTK_USE_MicronTracker */

namespace igstk
{

/** \class TrackerInitializer
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

class TrackerInitializer
{

public:
  
  typedef TrackerConfiguration::TrackerType  TrackerType;

  TrackerInitializer( );

  TrackerType GetTrackerType()
    {
    return m_TrackerType;
    }

  std::string GetTrackerTypeAsString();

  void SetTrackerConfiguration( TrackerConfiguration * config )
    {
    m_TrackerConfiguration = config;
    }

  int RequestInitializeTracker();
  
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

  void StopAndCloseTracker()
    {
    m_Tracker->RequestStopTracking();
    m_Tracker->RequestClose();
    if ( m_TrackerType == TrackerConfiguration::Aurora || 
         m_TrackerType == TrackerConfiguration::Polaris)
      {
      m_Communication->CloseCommunication();
      }
    
    }


  virtual ~TrackerInitializer();

private:
  
  TrackerType                           m_TrackerType;

  Tracker::Pointer                      m_Tracker;
  std::vector< TrackerTool::Pointer >   m_TrackerToolList;
  bool                                  m_HasReferenceTool;
  TrackerTool::Pointer                  m_ReferenceTool;

  PolarisTracker::Pointer               m_PolarisTracker;
  AuroraTracker::Pointer                m_AuroraTracker;
  SerialCommunication::Pointer          m_Communication;
  
  int InitializePolarisTracker();
  int InitializeAuroraTracker();

#ifdef IGSTK_USE_MicronTracker
  MicronTracker::Pointer                m_MicronTracker;
  int InitializeMicronTracker();
#endif /* IGSTK_USE_MicronTracker */
  

  TrackerConfiguration * m_TrackerConfiguration;
  
  std::string            m_ErrorMessage;

};

} // end of namespace
#endif
