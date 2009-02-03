PROJECT(TrackerConfiguration)

SET(PRECOMPUTED_TRANSFORM_READER_DIR
   ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
)

#
# Source code for the project
#
SET(SRCS
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkPrecomputedTransformData.cxx
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkTransformXMLFileReaderBase.cxx
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkRigidTransformXMLFileReader.cxx
    igstkTrackerConfiguration.cxx
    igstkAuroraTrackerConfiguration.cxx
    igstkMicronTrackerConfiguration.cxx
    igstkAscensionTrackerConfiguration.cxx
    igstkPolarisTrackerConfiguration.cxx
    igstkTrackerController.cxx
    configurationFileReaders/igstkTrackerConfigurationXMLFileReaderBase.cxx
    configurationFileReaders/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkPolarisWirelessConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkPolarisVicraConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkPolarisSpectraConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkPolarisHybridConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkAuroraConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkMicronConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkAscensionConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkTrackerConfigurationFileReader.cxx
)
SET(HDRS
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkPrecomputedTransformData.h
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkTransformXMLFileReaderBase.h
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkRigidTransformXMLFileReader.h
    igstkTrackerConfiguration.h
    igstkAuroraTrackerConfiguration.h
    igstkMicronTrackerConfiguration.h
    igstkAscensionTrackerConfiguration.h
    igstkPolarisTrackerConfiguration.h
    igstkTrackerController.h
    configurationFileReaders/igstkTrackerConfigurationXMLFileReaderBase.h
    configurationFileReaders/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h
    configurationFileReaders/igstkPolarisWirelessConfigurationXMLFileReader.h
    configurationFileReaders/igstkPolarisVicraConfigurationXMLFileReader.h
    configurationFileReaders/igstkPolarisSpectraConfigurationXMLFileReader.h
    configurationFileReaders/igstkPolarisHybridConfigurationXMLFileReader.h
    configurationFileReaders/igstkAuroraConfigurationXMLFileReader.h
    configurationFileReaders/igstkMicronConfigurationXMLFileReader.h
    configurationFileReaders/igstkAscensionConfigurationXMLFileReader.h
    configurationFileReaders/igstkTrackerConfigurationFileReader.h
)
#
#The directories containing all of the header files
#
INCLUDE_DIRECTORIES(
 ${PRECOMPUTED_TRANSFORM_READER_DIR}
 .
 ./configurationFileReaders
)

#
# Link to the following libraries
#
LINK_LIBRARIES (
  IGSTK
)

ADD_EXECUTABLE(TrackerConfigurationExample TrackerConfigurationExample.cxx ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(TrackerConfigurationExample ${LINK_LIBRARIES})
