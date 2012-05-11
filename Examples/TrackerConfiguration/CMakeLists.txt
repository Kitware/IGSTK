PROJECT(TrackerConfiguration)

SET(PRECOMPUTED_TRANSFORM_READER_DIR
   ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
)

#
# Source code for the project
#
SET(SRCS
    igstkTrackerConfiguration.cxx
    igstkAuroraTrackerConfiguration.cxx
    igstkMicronTrackerConfiguration.cxx
    igstkAscensionTrackerConfiguration.cxx
    igstkAscension3DGTrackerConfiguration.cxx
    igstkPolarisTrackerConfiguration.cxx
    igstkCertusTrackerConfiguration.cxx
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
    configurationFileReaders/igstkAscension3DGConfigurationXMLFileReader.cxx
    configurationFileReaders/igstkTrackerConfigurationFileReader.cxx
    configurationFileReaders/igstkCertusConfigurationXMLFileReader.cxx
)
SET(HDRS
    igstkTrackerConfiguration.h
    igstkAuroraTrackerConfiguration.h
    igstkMicronTrackerConfiguration.h
    igstkAscensionTrackerConfiguration.h
    igstkAscension3DGTrackerConfiguration.h
    igstkPolarisTrackerConfiguration.h
    igstkCertusTrackerConfiguration.h
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
    configurationFileReaders/igstkAscension3DGConfigurationXMLFileReader.h
    configurationFileReaders/igstkTrackerConfigurationFileReader.h
   configurationFileReaders/igstkCertusConfigurationXMLFileReader.h
)
#
#The directories containing all of the header files
#
INCLUDE_DIRECTORIES(
 ${PRECOMPUTED_TRANSFORM_READER_DIR}
 .
 ./configurationFileReaders
)

ADD_LIBRARY(TrackerConfiguration ${SRCS} ${HDRS})

ADD_EXECUTABLE(TrackerConfigurationExample TrackerConfigurationExample.cxx)
TARGET_LINK_LIBRARIES(TrackerConfigurationExample TrackerConfiguration TransformReaders IGSTK )

