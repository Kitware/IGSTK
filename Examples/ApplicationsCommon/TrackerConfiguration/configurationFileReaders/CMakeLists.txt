PROJECT(configurationFileReaders)
  
  
SET(configurationFileReaders_SRCS
  igstkAscensionConfigurationXMLFileReader.cxx
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
  ${configurationFileReaders_SOURCE_DIR}
  ${configurationFileReaders_BINARY_DIR}
  ${CalibrationRegistrationLoading_SOURCE_DIR}
  )

ADD_LIBRARY(configurationFileReaders ${configurationFileReaders_SRCS})
TARGET_LINK_LIBRARIES(configurationFileReaders TrackerConfiguration CalibrationRegistrationLoading IGSTKSandbox)


