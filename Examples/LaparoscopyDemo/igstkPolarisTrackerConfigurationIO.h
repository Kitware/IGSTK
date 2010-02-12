/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkPolarisTrackerConfigurationIO.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPolarisTrackerConfigurationIO_h
#define __igstkPolarisTrackerConfigurationIO_h


#include "igstkTrackerConfiguration.h"

namespace igstk
{

/** \class PolarisTrackerConfigurationIO
* 
* \brief Reader/Writer class for PolarisTrackerConfiguration
*
* Sample format:
---------------------------------------------------------------------
# Serial Port Number
0,1,2...
# Grasper Tool SROM file
SROM File
# Tip Offset
x y z
# Scope Tool SROM file
SROM File
# Tip Offset
x y z
--------------------------------------------------------------------
*/

class PolarisTrackerConfigurationIO
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

  NDITrackerConfiguration * GetPolarisTrackerConfiguration()
    {
    return m_PolarisTrackerConfiguration;
    }

  void SetPolarisTrackerConfiguration(NDITrackerConfiguration * ini)
    {
    m_PolarisTrackerConfiguration = ini;
    }

  PolarisTrackerConfigurationIO();
  virtual ~PolarisTrackerConfigurationIO();

private:
  NDITrackerConfiguration         * m_PolarisTrackerConfiguration;
  std::string             m_FileName;

};

} // end of namespace
#endif
