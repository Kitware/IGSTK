/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    TrackerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __TrackerConfiguration_h
#define __TrackerConfiguration_h

#include "igstkSerialCommunication.h"

class MicronTrackerConfiguration;
class MicronTrackerToolConfiguration;
class NDITrackerConfiguration;
class NDITrackerToolConfiguration;


class TrackerConfiguration
{

public:

  enum TrackerType { Polaris,
                     Aurora,
                     Micron};
    
  TrackerConfiguration( TrackerType trackerType);

  TrackerType GetTrackerType() { return m_TrackerType; };

  std::string GetTrackerTypeAsString();

  MicronTrackerConfiguration * GetMicronTrackerConfiguration()
  {
    return m_MicronTrackerConfiguration;
  }

  NDITrackerConfiguration    * GetNDITrackerConfiguration()
  {
    return m_NDITrackerConfiguration;
  }

  virtual ~TrackerConfiguration();

private:
  
  TrackerType                  m_TrackerType;

  MicronTrackerConfiguration * m_MicronTrackerConfiguration;
  NDITrackerConfiguration    * m_NDITrackerConfiguration;

};


class MicronTrackerToolConfiguration
{
public:
  std::string        MarkerName;
  bool               IsReference;
  igstk::Transform   CalibrationTransform;

  MicronTrackerToolConfiguration(){};
  virtual ~MicronTrackerToolConfiguration(){};
};

class NDITrackerToolConfiguration
{
public:
  bool               WiredTool;
  bool               Is5DOF;
  bool               IsReference;
  int                PortNumber;
  int                ChannelNumber;
  std::string        SROMFile;
  igstk::Transform   CalibrationTransform;

  NDITrackerToolConfiguration(){};
  virtual ~NDITrackerToolConfiguration(){};
};


class MicronTrackerConfiguration
{
public:
  std::string     CameraCalibrationFileDirectory;
  std::string     InitializationFile;
  std::string     TemplateDirectory;

  std::list< MicronTrackerToolConfiguration * > TrackerToolList;

  MicronTrackerConfiguration(){};
  virtual ~MicronTrackerConfiguration(){};

};

class NDITrackerConfiguration
{
public:
  igstk::SerialCommunication::PortNumberType     COMPort;

  std::list< NDITrackerToolConfiguration * >     TrackerToolList;

  NDITrackerConfiguration(){};
  virtual ~NDITrackerConfiguration(){};
};
#endif
