/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkArucoTrackerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkArucoTrackerConfiguration_h
#define __igstkArucoTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


/**
 * \class ArucoToolConfiguration
 * \brief This class represents the configuration data required by an
 * ArucoTracker tool.
 *
 * This class represents the configuration data required by an ArucoTracker
 * tool.
 */
class ArucoToolConfiguration : public TrackerToolConfiguration
{
public:
  ArucoToolConfiguration();
  ArucoToolConfiguration(const ArucoToolConfiguration &other);
  virtual ~ArucoToolConfiguration();

  igstkSetMacro( MarkerID, unsigned int );
  igstkGetMacro( MarkerID, unsigned int );
  virtual std::string GetToolTypeAsString();

protected:
  unsigned int m_MarkerID;
};


/**
 * \class ArucoTrackerConfiguration This class represents the configuration
 *        data required by the Aruco tracker.
 */
class ArucoTrackerConfiguration : public TrackerConfiguration
{
public:
  //standard typedefs
  igstkStandardClassBasicTraitsMacro( ArucoTrackerConfiguration,
                                      TrackerConfiguration )

  //method for creation through the object factory
  igstkNewMacro( Self );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

  igstkSetMacro( CameraCalibrationFile, std::string );
  igstkGetMacro( CameraCalibrationFile, std::string );

  igstkSetMacro( MarkerSize, unsigned int );
  igstkGetMacro( MarkerSize, unsigned int );

protected:
  virtual void InternalAddTool( const TrackerToolConfiguration *tool,
                                bool isReference );
  ArucoTrackerConfiguration();
  virtual ~ArucoTrackerConfiguration();

private:
  std::string     m_CameraCalibrationFile;
  unsigned int    m_MarkerSize;

  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;
};


} // end of name space
#endif
