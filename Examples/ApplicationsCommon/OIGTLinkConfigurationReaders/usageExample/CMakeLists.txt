cmake_minimum_required(VERSION 2.4)

PROJECT(OIGTLinkTrackerConfigurationExample)

#
#  FIND ITK
#
FIND_PACKAGE(ITK)
IF (USE_ITK_FILE)
  INCLUDE (${USE_ITK_FILE})
ELSE (USE_ITK_FILE)
  MESSAGE( FATAL_ERROR "This application requires ITK. One of these components is missing. Please verify configuration")
ENDIF (USE_ITK_FILE)

#
#  FIND VTK
#
FIND_PACKAGE(VTK)
IF (USE_VTK_FILE)
  INCLUDE (${USE_VTK_FILE})
ELSE (USE_VTK_FILE)
  MESSAGE( FATAL_ERROR "This application requires VTK. One of these components is missing. Please verify configuration")
ENDIF (USE_VTK_FILE)

#
#  FIND FLTK
#
FIND_PACKAGE(FLTK)
 IF (FLTK_FOUND)
  INCLUDE_DIRECTORIES (${FLTK_INCLUDE_DIR})
ELSE (FLTK_FOUND)
  MESSAGE( FATAL_ERROR "This application requires FLTK. One of these components is missing. Please verify configuration")
ENDIF (FLTK_FOUND)

#
#  FIND IGSTKSandbox
#
FIND_PACKAGE(IGSTKSandbox)
IF (IGSTKSandbox_USE_FILE)
  INCLUDE (${IGSTKSandbox_USE_FILE})
ELSE (IGSTKSandbox_USE_FILE)
  MESSAGE( FATAL_ERROR "This application requires IGSTK. One of these components is missing. Please verify configuration")
ENDIF (IGSTKSandbox_USE_FILE)

SET(PRECOMPUTED_TRANSFORM_READER_DIR
    C:/toolkits/IGSTK/IGSTKSandbox_SRC/Examples/ApplicationsCommon/CalibrationRegistrationLoading
)

SET(TRACKER_CONFIGURATION_DIR
    ../../..
)

SET(TRACKER_CONFIGURATION_READERS_DIR
    ../..
)

SET(OIGTLINK_CONFIGURATION_READERS_DIR
    ..
)


#
# Source code for the project
#
SET(SRCS
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkPrecomputedTransformData.cxx
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkTransformXMLFileReaderBase.cxx
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkRigidTransformXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_DIR}/igstkTrackerConfiguration.cxx
    ${TRACKER_CONFIGURATION_DIR}/igstkAuroraTrackerConfiguration.cxx
    ${TRACKER_CONFIGURATION_DIR}/igstkMicronTrackerConfiguration.cxx
    ${TRACKER_CONFIGURATION_DIR}/igstkPolarisTrackerConfiguration.cxx
    ${TRACKER_CONFIGURATION_DIR}/igstkTrackerController.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkTrackerConfigurationXMLFileReaderBase.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisWirelessConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisVicraConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisSpectraConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisHybridConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkAuroraConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkMicronConfigurationXMLFileReader.cxx
    ${OIGTLINK_CONFIGURATION_READERS_DIR}/igstkOIGTLinkTrackerConfigurationFileReader.cxx
    ${OIGTLINK_CONFIGURATION_READERS_DIR}/igstkOIGTLinkTrackerConfigurationXMLFileReader.cxx
    main.cxx
)
SET(HDRS
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkPrecomputedTransformData.h
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkTransformXMLFileReaderBase.h
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkRigidTransformXMLFileReader.h
    ${TRACKER_CONFIGURATION_DIR}/igstkTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkAuroraTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkMicronTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkPolarisTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkTrackerController.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkTrackerConfigurationXMLFileReaderBase.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisWirelessConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisVicraConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisSpectraConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisHybridConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkAuroraConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkMicronConfigurationXMLFileReader.h
    ${OIGTLINK_CONFIGURATION_READERS_DIR}/igstkOIGTLinkTrackerConfigurationFileReader.h
    ${OIGTLINK_CONFIGURATION_READERS_DIR}/igstkOIGTLinkTrackerConfigurationXMLFileReader.h
)

#
#The directories containing all of the header files
#
INCLUDE_DIRECTORIES(
${TRACKER_CONFIGURATION_DIR}
${TRACKER_CONFIGURATION_READERS_DIR}
${OIGTLINK_CONFIGURATION_READERS_DIR}
${PRECOMPUTED_TRANSFORM_READER_DIR}
${IGSTKSandbox_INCLUDE_DIR}
)


#
# Link to the following libraries
#
LINK_LIBRARIES (
  IGSTKSandbox
)


ADD_EXECUTABLE(oigtLinkExample ${SRCS} ${HDRS})

TARGET_LINK_LIBRARIES(oigtLinkExample ${LINK_LIBRARIES})
