PROJECT(OIGTLinkConfigurationReaders)
  
  
SET(OIGTLinkConfigurationReaders_SRCS
  igstkOIGTLinkTrackerConfigurationXMLFileReader.cxx
  igstkOIGTLinkTrackerConfigurationFileReader.cxx
)
  
INCLUDE_DIRECTORIES(
  ${OIGTLinkConfigurationReaders_SOURCE_DIR}
  ${OIGTLinkConfigurationReaders_BINARY_DIR}
  ${CalibrationRegistrationLoading_SOURCE_DIR}
  ${TrackerConfiguration_SOURCE_DIR}
  ${TrackerConfiguration_BINARY_DIR}
  ${ConfigurationFileReaders_SOURCE_DIR}
  ${ConfigurationFileReaders_BINARY_DIR}
  )

ADD_LIBRARY(OIGTLinkConfigurationReaders ${OIGTLinkConfigurationReaders_SRCS})
TARGET_LINK_LIBRARIES(OIGTLinkConfigurationReaders TrackerConfiguration CalibrationRegistrationLoading ConfigurationFileReaders IGSTKSandbox)


