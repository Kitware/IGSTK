#include "igstkPolarisSpectraConfigurationXMLFileReader.h"
#include "igstkPolarisTrackerConfiguration.h"

namespace igstk
{
double 
PolarisSpectraConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::PolarisSpectraTrackerConfiguration trackerConfig;

  return trackerConfig.GetMaximalRefreshRate();
}


std::string 
PolarisSpectraConfigurationXMLFileReader::GetSystemType()
{
  return "polaris wireless spectra";
}

igstk::PolarisWirelessTrackerConfiguration * 
PolarisSpectraConfigurationXMLFileReader::GetPolarisConfiguration()
{
  return new igstk::PolarisSpectraTrackerConfiguration();
}

} //namespace
