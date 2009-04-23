PROJECT(OpenIGTLinkConfigurationReaders)
  
  
SET(OpenIGTLinkConfigurationReaders_SRCS
  igstkOpenIGTLinkTrackerConfigurationXMLFileReader.cxx
  igstkOpenIGTLinkTrackerConfigurationFileReader.cxx
)
  
INCLUDE_DIRECTORIES(
  ${OpenIGTLinkConfigurationReaders_SOURCE_DIR}
  ${OpenIGTLinkConfigurationReaders_BINARY_DIR}
  ${CalibrationRegistrationLoading_SOURCE_DIR}
  ${TrackerConfiguration_SOURCE_DIR}
  ${TrackerConfiguration_BINARY_DIR}
  ${ConfigurationFileReaders_SOURCE_DIR}
  ${ConfigurationFileReaders_BINARY_DIR}
  )

ADD_LIBRARY(OpenIGTLinkConfigurationReaders ${OpenIGTLinkConfigurationReaders_SRCS})
TARGET_LINK_LIBRARIES(OpenIGTLinkConfigurationReaders TrackerConfiguration CalibrationRegistrationLoading ConfigurationFileReaders IGSTKSandbox)


