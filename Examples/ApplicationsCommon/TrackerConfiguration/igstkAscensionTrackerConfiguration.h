/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionTrackerConfiguration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

     =========================================================================*/

#ifndef __igstkAscensionTrackerConfiguration_h
#define __igstkAscensionTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


/**
 * \class AscensionToolConfiguration 
 * \brief This class represents the configuration data required by an Ascension 
 * FOB tool. 
 *
 * This class represents the configuration data required by an Ascension 
 * FOB tool. By default the tool is assumed to be connected on bird port 1.
 */
class AscensionToolConfiguration : public TrackerToolConfiguration
{
public:
  AscensionToolConfiguration();
  AscensionToolConfiguration(const AscensionToolConfiguration &other);
  virtual ~AscensionToolConfiguration();

  igstkSetMacro( PortNumber, unsigned int );
  igstkGetMacro( PortNumber, unsigned int );

  virtual std::string GetToolTypeAsString();

protected:

  unsigned int m_PortNumber;
};



/**
 * Configuration for the Ascension tracker.
 */
class AscensionTrackerConfiguration : 
  public SerialCommunicatingTrackerConfiguration
{
public:

   //standard typedefs
  igstkStandardClassBasicTraitsMacro( AscensionTrackerConfiguration, 
                                      SerialCommunicatingTrackerConfiguration )
  //method for creation through the object factory
  igstkNewMacro( Self );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:

  AscensionTrackerConfiguration();
  virtual ~AscensionTrackerConfiguration();

  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );

private:
                  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;

                   //maximal number of physical ports 
  static const unsigned MAXIMAL_PORT_NUMBER;
};

} // end of name space
#endif
