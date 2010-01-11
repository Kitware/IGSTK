/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkMicronTrackerConfigurationIO.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMicronTrackerConfigurationIO_h
#define __igstkMicronTrackerConfigurationIO_h


#include "igstkTrackerConfiguration.h"

namespace igstk
{

/** \class MicronTrackerConfigurationIO
* 
* \brief Reader/Writer class for MicronTrackerConfiguration
*
* Sample format:
---------------------------------------------------------------------
# Calibration file dir
calibration_dir
# Ini file
ini_file
# Marker dir
marker_dir
# Tracker tool marker name
marker_name
# Use reference tool?
0 or 1
# Reference tool marker name
marker_name
--------------------------------------------------------------------
*/

class MicronTrackerConfigurationIO
{

public:
  
  std::string GetFileName()
    {
    return m_FileName;
    }

  void SetFileName( std::string fileName)
    {
    m_FileName = fileName;
    }

  int RequestRead();
  int RequestWrite();

  MicronTrackerConfiguration * GetMicronTrackerConfiguration()
    {
    return m_MicronTrackerConfiguration;
    }

  void SetMicronTrackerConfiguration(MicronTrackerConfiguration * ini)
    {
    m_MicronTrackerConfiguration = ini;
    }

  MicronTrackerConfigurationIO();
  virtual ~MicronTrackerConfigurationIO();

private:
  MicronTrackerConfiguration         * m_MicronTrackerConfiguration;
  std::string             m_FileName;

};

} // end of namespace
#endif
