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
