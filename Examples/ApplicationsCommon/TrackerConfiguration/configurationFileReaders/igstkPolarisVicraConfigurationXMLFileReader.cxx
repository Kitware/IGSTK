/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisVicraConfigurationXMLFileReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPolarisVicraConfigurationXMLFileReader.h"
#include "igstkPolarisTrackerConfiguration.h"

namespace igstk
{
double 
PolarisVicraConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::PolarisVicraTrackerConfiguration::Pointer 
    trackerConfig = igstk::PolarisVicraTrackerConfiguration::New();

  return trackerConfig->GetMaximalRefreshRate();
}


std::string 
PolarisVicraConfigurationXMLFileReader::GetSystemType()
{
  return "polaris vicra";
}


igstk::PolarisWirelessTrackerConfiguration::Pointer
PolarisVicraConfigurationXMLFileReader::GetPolarisConfiguration()
{
  //explicitly upcast to avoid the compiler warning
  igstk::PolarisWirelessTrackerConfiguration::Pointer polarisWireless;
  polarisWireless  = PolarisVicraTrackerConfiguration::New();
  return polarisWireless;
}

} //namespace
