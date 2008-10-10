/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkMicronTrackerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMicronTrackerConfiguration_h
#define __igstkMicronTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


/**
 * A micron tracker tool. 
 * NOTE: The tool name, TrackerToolConfiguration::SetToolName(), has to be
 *       the same as the marker template file name.
 */
class MicronToolConfiguration : public TrackerToolConfiguration
{
public:
  MicronToolConfiguration();
  MicronToolConfiguration(const MicronToolConfiguration &other);
  virtual ~MicronToolConfiguration();

  virtual std::string GetToolTypeAsString();
};


/**
 * Configuration for the Micron tracker.
 */
class MicronTrackerConfiguration : public TrackerConfiguration
{
public:
  MicronTrackerConfiguration();
  virtual ~MicronTrackerConfiguration();

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

  igstkSetMacro( CameraCalibrationFileDirectory, std::string );
  igstkGetMacro( CameraCalibrationFileDirectory, std::string );

  igstkSetMacro( InitializationFile, std::string );
  igstkGetMacro( InitializationFile, std::string );

  igstkSetMacro( TemplatesDirectory, std::string );
  igstkGetMacro( TemplatesDirectory, std::string );

protected:
  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );

private:
  std::string     m_CameraCalibrationFileDirectory;
  std::string     m_InitializationFile;
  std::string     m_TemplatesDirectory;  

                  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;
};


} // end of name space
#endif
