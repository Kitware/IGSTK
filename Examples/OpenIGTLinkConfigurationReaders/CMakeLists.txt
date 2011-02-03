PROJECT(OpenIGTLinkConfigurationReaders)
  
SET(PRECOMPUTED_TRANSFORM_READER_DIR
   ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
)

SET(TRACKER_CONFIGURATION_DIR
   ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration
)

SET(TRACKER_CONFIGURATION_READER_DIR
   ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders
)

 
INCLUDE_DIRECTORIES(
  ${OpenIGTLinkConfigurationReaders_SOURCE_DIR}
  ${OpenIGTLinkConfigurationReaders_BINARY_DIR}
  ${TRACKER_CONFIGURATION_DIR}
  ${TRACKER_CONFIGURATION_READER_DIR}
  ${PRECOMPUTED_TRANSFORM_READER_DIR}
  )

SET(OpenIGTLinkConfigurationReaders_SRCS
  igstkOpenIGTLinkTrackerConfigurationXMLFileReader.cxx
  igstkOpenIGTLinkTrackerConfigurationFileReader.cxx
)

ADD_LIBRARY(OpenIGTLinkConfigurationReaders ${OpenIGTLinkConfigurationReaders_SRCS} )


