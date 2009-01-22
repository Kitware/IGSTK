/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisSpectraConfigurationXMLFileReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPolarisSpectraConfigurationXMLFileReader.h"
#include "igstkPolarisTrackerConfiguration.h"

namespace igstk
{
double 
PolarisSpectraConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::PolarisSpectraTrackerConfiguration::Pointer trackerConfig=
    igstk::PolarisSpectraTrackerConfiguration::New();

  return trackerConfig->GetMaximalRefreshRate();
}


std::string 
PolarisSpectraConfigurationXMLFileReader::GetSystemType()
{
  return "polaris wireless spectra";
}

igstk::PolarisWirelessTrackerConfiguration::Pointer 
PolarisSpectraConfigurationXMLFileReader::GetPolarisConfiguration()
{
  //explicitly upcast to avoid the compiler warning
  igstk::PolarisWirelessTrackerConfiguration::Pointer polarisWireless;
  polarisWireless = PolarisSpectraTrackerConfiguration::New();
  return polarisWireless;
}

} //namespace
