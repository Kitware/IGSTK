/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerConfiguration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAuroraTrackerConfiguration_h
#define __igstkAuroraTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


 /**
 * \class AuroraToolConfiguration 
 * \brief This class represents the configuration data required by an Aurora 
 * tool. 
 *
 * This class represents the configuration data required by an Aurora tool.
 * By default the tool is assumed to be a 6DOF tool connected to
 * the control box on physical port 1. To indicate that the tool is a 5DOF tool
 * the user sets the tool's channel which has the side effect of indicating that
 * the tool is a 5DOF tool.
 */
class AuroraToolConfiguration : public TrackerToolConfiguration
{
public:
  AuroraToolConfiguration();
  AuroraToolConfiguration(const AuroraToolConfiguration &other);
  virtual ~AuroraToolConfiguration();

  /**
   * Set the tool's physical port number. Valid values are 1-4.
   */
  igstkSetMacro( PortNumber, unsigned );
  igstkGetMacro( PortNumber, unsigned );

  /**
   * Set the tool's channel number on the specific port. Valid values are 0,1.
   * By default this value is 0. If a 5DOF tool is connected via a splitter it
   * can be 1.
   * NOTE: Invoking this method implies that this is a 5DOF tool.
   */
  void SetChannelNumber( unsigned channelNumber );  
  igstkGetMacro( ChannelNumber, unsigned );

  igstkSetMacro( SROMFile, std::string );
  igstkGetMacro( SROMFile, std::string );

  igstkGetMacro( Is5DOF, bool );

  virtual std::string GetToolTypeAsString();

protected:
  //physical port to which the wired tool is attached
  unsigned     m_PortNumber;

  //if this is a five degree of freedom tool it can be attached to
  //the toolbox with a splitter that supports two 5DOF tools on a 
  //single port, we then have to specify the channel [0,1]
  unsigned     m_ChannelNumber;

  bool         m_Is5DOF;

  //SROM file to use with the tool, required for wireless, optional for
  //wired
  std::string  m_SROMFile;
};


/**
 * \class AuroraTrackerConfiguration This class represents the configuration 
 *        data required by the Aurora tracker.
 */
class AuroraTrackerConfiguration : 
  public SerialCommunicatingTrackerConfiguration
{
public:
  //standard typedefs
  igstkStandardClassBasicTraitsMacro( AuroraTrackerConfiguration, 
                                      SerialCommunicatingTrackerConfiguration )
  //method for creation through the object factory
  igstkNewMacro( Self );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

 protected:

  AuroraTrackerConfiguration();
  virtual ~AuroraTrackerConfiguration();

  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );

private:
  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;

  //maximal number of physical ports 
  static const unsigned MAXIMAL_PORT_NUMBER;

  //maximal channel number on port
  static const unsigned MAXIMAL_CHANNEL_NUMBER;
};

} // end of name space
#endif
