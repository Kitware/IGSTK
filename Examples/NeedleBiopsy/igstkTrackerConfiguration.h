/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkTrackerConfiguration_h
#define __igstkTrackerConfiguration_h

#include "igstkSerialCommunication.h"

namespace igstk
{

class MicronTrackerConfiguration;
class MicronTrackerToolConfiguration;
class NDITrackerConfiguration;
class NDITrackerToolConfiguration;


/** \class TrackerConfiguration
* 
* \brief Aggregate class for different tracker configurations.
*
* This is a generic class encapsulates the complexity of different trackers.
* It functions as an adapter between TrackerConfigurationGUI classes and
* TrackerInitializer class.
*
* Currently, it has internal pointer to NDITrackers (Polaris and Aurora) and
* MicronTracker. Which can be Set/Get using API methods.
*
* In this header files, we also defined classes for each supported trackers.
* Each tracker configuration contains information for setting up the tracker.
* It also keeps an internal list of TrackerToolConfiguration, which stores 
* information for all the tracker tools.
*
*/
class TrackerConfiguration
{

public:

  enum TrackerType { Polaris,
                     Aurora,
                     Micron};
    
  TrackerConfiguration();

  TrackerType GetTrackerType() { return m_TrackerType; }

  void SetTrackerType( TrackerType trackerType );

  std::string GetTrackerTypeAsString();

  MicronTrackerConfiguration * GetMicronTrackerConfiguration()
    {
    return m_MicronTrackerConfiguration;
    }

  void SetMicronTrackerConfiguration(MicronTrackerConfiguration * conf)
    {
    m_MicronTrackerConfiguration = conf;
    }

  NDITrackerConfiguration    * GetNDITrackerConfiguration()
    {
    return m_NDITrackerConfiguration;
    }

  void SetNDITrackerConfiguration(NDITrackerConfiguration * conf)
    {
    m_NDITrackerConfiguration = conf;
    }

  // Check the validity of the configuration, not yet implemented
  int CheckValidity();

  virtual ~TrackerConfiguration();

private:
  int CheckPolarisValidity();
  int CheckAuroraValidity();
  int CheckMicronValidity();

  TrackerType                  m_TrackerType;
  std::string                  m_ValidityErrorMessage;

  MicronTrackerConfiguration * m_MicronTrackerConfiguration;
  NDITrackerConfiguration    * m_NDITrackerConfiguration;

};


class MicronTrackerToolConfiguration
{
public:
  std::string        m_MarkerName;
  bool               m_IsReference;
  igstk::Transform   m_CalibrationTransform;

  MicronTrackerToolConfiguration()
    {
    m_MarkerName  = "";
    m_IsReference = 0;
    m_CalibrationTransform.SetToIdentity(
      igstk::TimeStamp::GetLongestPossibleTime());
    }
  virtual ~MicronTrackerToolConfiguration(){};
};

class NDITrackerToolConfiguration
{
public:
  bool               m_WiredTool;
  bool               m_Is5DOF;
  bool               m_IsReference;
  bool               m_HasSROM;
  int                m_PortNumber;
  int                m_ChannelNumber;
  std::string        m_SROMFile;
  igstk::Transform   m_CalibrationTransform;

  NDITrackerToolConfiguration()
    {
    m_WiredTool    = 1;
    m_Is5DOF       = 0;
    m_IsReference  = 0;
    m_HasSROM      = 0;
    m_PortNumber   = 0;
    m_ChannelNumber= 0;
    m_SROMFile     = "";
    m_CalibrationTransform.SetToIdentity(
      igstk::TimeStamp::GetLongestPossibleTime());
    }
  virtual ~NDITrackerToolConfiguration(){};
};


class MicronTrackerConfiguration
{
public:
  std::string     m_CameraCalibrationFileDirectory;
  std::string     m_InitializationFile;
  std::string     m_TemplatesDirectory;
  double          m_Frequency;

  std::vector< MicronTrackerToolConfiguration * > m_TrackerToolList;

  MicronTrackerConfiguration()
    {
    m_CameraCalibrationFileDirectory = "";
    m_InitializationFile             = "";
    m_TemplatesDirectory             = "";
    m_Frequency = 30;
    m_TrackerToolList.clear();
    }
  virtual ~MicronTrackerConfiguration(){};

};

class NDITrackerConfiguration
{
public:
  igstk::SerialCommunication::PortNumberType     m_COMPort;
  double                                         m_Frequency;

  std::vector< NDITrackerToolConfiguration * >   m_TrackerToolList;

  NDITrackerConfiguration()
    {
    m_COMPort   = SerialCommunication::PortNumber0; 
    m_Frequency = 30;
    m_TrackerToolList.clear();
    }
  virtual ~NDITrackerConfiguration(){};
};

} // end of name space
#endif
