/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkAscension3DGTrackerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAscension3DGTrackerConfiguration_h
#define __igstkAscension3DGTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


/**
 * \class Ascension3DGToolConfiguration 
 * \brief This class represents the configuration data required by a Ascension3DG 
 * tool. 
 *
 * This class represents the configuration data required by a Ascension3DG tool.
 * NOTE: The tool and marker names are two distinct entities. The tool name 
 *       is the name the user decides to assign the specific tool (US, bayonet
 *       probe, etc.). The marker name is the name of the tracked marker which
 *       is attached to the tool.
 */
class Ascension3DGToolConfiguration : public TrackerToolConfiguration
{
public:
  Ascension3DGToolConfiguration();
  virtual ~Ascension3DGToolConfiguration();

  igstkSetMacro( PortNumber, unsigned int );
  igstkGetMacro( PortNumber, unsigned int );
  
  virtual std::string GetToolTypeAsString();

protected:
  unsigned int m_PortNumber;
};


/**
 * \class Ascension3DGTrackerConfiguration This class represents the configuration 
 *        data required by the Ascension3DG tracker.
 */
class Ascension3DGTrackerConfiguration : public TrackerConfiguration
{
public:
  //standard typedefs
  igstkStandardClassBasicTraitsMacro( Ascension3DGTrackerConfiguration, 
                                      TrackerConfiguration )

  //method for creation through the object factory
  igstkNewMacro( Self );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );
  Ascension3DGTrackerConfiguration();
  virtual ~Ascension3DGTrackerConfiguration();

private:

  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;
};


} // end of name space
#endif
