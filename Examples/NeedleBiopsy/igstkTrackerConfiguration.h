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
#ifndef __TrackerConfiguration_h
#define __TrackerConfiguration_h

#include "igstkSerialCommunication.h"

namespace igstk
{

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
    
  TrackerConfiguration();

  TrackerType GetTrackerType() { return m_TrackerType; };

  void SetTrackerType( TrackerType trackerType );
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
  bool               HasSROM;
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
  std::string     TemplatesDirectory;
  double          Frequency;

  std::vector< MicronTrackerToolConfiguration * > TrackerToolList;

  MicronTrackerConfiguration()
  {
    Frequency = 30;
  }
  virtual ~MicronTrackerConfiguration(){};

};

class NDITrackerConfiguration
{
public:
  igstk::SerialCommunication::PortNumberType     COMPort;
  double                                         Frequency;

  std::vector< NDITrackerToolConfiguration * >     TrackerToolList;

  NDITrackerConfiguration()
  {
    Frequency = 30;
  }
  virtual ~NDITrackerConfiguration(){};
};

} // end of name space
#endif