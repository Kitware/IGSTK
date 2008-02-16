/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerConfigurationGUIBase.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkTrackerConfigurationGUIBase_h
#define __igstkTrackerConfigurationGUIBase_h

#include "igstkTrackerConfiguration.h"
#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"

namespace igstk
{

/** \class TrackerConfigurationGUIBase
* 
* \brief Base class for different tracker configuration GUIs.
*
* This is a base class defines common functionalities of different tracker
* configuration GUIs.
* 
* SetConfiguration: 
*        Will allow the GUI to initialize itself according the configuration 
*        passed to it. sub-class should implement the InitializeGUI method 
*        to achieve this.
*
* RequestGetConfiguration:
*         This will invoke ConfigurationEvent with configuration as payload.
*         User should setup an observer to listen to this event.
*
* GetGUISetting:
*         User need to implement this methods in the sub-classes for tracker
*         specific GUI. For example: For Aurora tracker, you need to specify
*         port number for tracker tool; for Micron tracker, you need to 
*         specify marker name. This methods will be called to update the 
*         Internal configuration object before it's been send out in event.
*
* Refer to:
*         PolarisTrackerConfigurationGUI.fl
*         AuroraTrackerConfigurationGUI.fl
*         MicronTrackerConfigurationGUI.fl
*/
class TrackerConfigurationGUIBase
{

public:
  
  typedef TrackerConfiguration::TrackerType TrackerType;

  TrackerType GetTrackerType() { return m_TrackerType; }

  virtual void SetConfiguration( TrackerConfiguration config )
    {
    m_TrackerConfiguration = config;
    InitializeGUI();
    }

  virtual void RequestGetConfiguration()
    {
    GetGUISetting();
    ConfigurationEvent confEvent;
    confEvent.Set( m_TrackerConfiguration );
    m_Reporter->InvokeEvent( confEvent );
    }

  igstkLoadedEventMacro( ConfigurationEvent, IGSTKEvent, TrackerConfiguration );

  unsigned long AddObserver( 
    const ::itk::EventObject & event, ::itk::Command * observer );

  void RemoveObserver( unsigned long tag );
  void RemoveAllObservers();

  TrackerConfigurationGUIBase();
  virtual ~TrackerConfigurationGUIBase();

protected:
  
  // Tracker specific GUI class has to implement this methods
  virtual void InitializeGUI() {};
  virtual void GetGUISetting() {};
  TrackerConfiguration      m_TrackerConfiguration;

private:
  
  TrackerType               m_TrackerType;
  ::itk::Object::Pointer    m_Reporter;

};

} // end of name space
#endif
