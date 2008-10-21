#include "igstkPolarisVicraConfigurationXMLFileReader.h"
#include "igstkPolarisTrackerConfiguration.h"

namespace igstk
{
double 
PolarisVicraConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::PolarisVicraTrackerConfiguration trackerConfig;

  return trackerConfig.GetMaximalRefreshRate();
}


std::string 
PolarisVicraConfigurationXMLFileReader::GetSystemType()
{
  return "polaris vicra";
}


igstk::PolarisWirelessTrackerConfiguration * 
PolarisVicraConfigurationXMLFileReader::GetPolarisConfiguration()
{
  return new igstk::PolarisVicraTrackerConfiguration();
}

} //namespace
