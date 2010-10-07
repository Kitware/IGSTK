PROJECT(ConfigurationFileReaders)
  
  
SET(ConfigurationFileReaders_SRCS
  igstkAscensionConfigurationXMLFileReader.cxx
  igstkAscension3DGConfigurationXMLFileReader.cxx
  igstkAuroraConfigurationXMLFileReader.cxx
  igstkMicronConfigurationXMLFileReader.cxx
  igstkPolarisHybridConfigurationXMLFileReader.cxx
  igstkPolarisSpectraConfigurationXMLFileReader.cxx
  igstkPolarisVicraConfigurationXMLFileReader.cxx
  igstkPolarisWirelessConfigurationXMLFileReader.cxx
  igstkSerialCommunicatingTrackerConfigurationXMLFileReader.cxx
  igstkTrackerConfigurationFileReader.cxx
  igstkTrackerConfigurationXMLFileReaderBase.cxx
)
  
INCLUDE_DIRECTORIES(
  ${ConfigurationFileReaders_SOURCE_DIR}
  ${ConfigurationFileReaders_BINARY_DIR}
  ${CalibrationRegistrationLoading_SOURCE_DIR}
  ${TrackerConfiguration_SOURCE_DIR}
  ${TrackerConfiguration_BINARY_DIR}
  )

ADD_LIBRARY(ConfigurationFileReaders ${ConfigurationFileReaders_SRCS})
TARGET_LINK_LIBRARIES(ConfigurationFileReaders TrackerConfiguration CalibrationRegistrationLoading IGSTKSandbox)


