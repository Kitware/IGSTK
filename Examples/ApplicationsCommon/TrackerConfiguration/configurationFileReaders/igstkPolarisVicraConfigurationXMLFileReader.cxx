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
  igstk::PolarisWirelessTrackerConfiguration::Pointer 
    polarisWireless = PolarisVicraTrackerConfiguration::New();
  return polarisWireless;
}

} //namespace
