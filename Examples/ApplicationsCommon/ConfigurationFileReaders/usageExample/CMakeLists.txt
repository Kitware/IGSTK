cmake_minimum_required(VERSION 2.4)

PROJECT(TrackerConfigurationExample)

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
   ${PROJECT_SOURCE_DIR}/../../../CalibrationRegistrationLoading
)

#
# Source code for the project
#
SET(SRCS
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkPrecomputedTransformData.cxx
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkTransformXMLFileReaderBase.cxx
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkRigidTransformXMLFileReader.cxx
    ../../igstkTrackerConfiguration.cxx
    ../../igstkAuroraTrackerConfiguration.cxx
    ../../igstkMicronTrackerConfiguration.cxx
    ../../igstkPolarisTrackerConfiguration.cxx
    ../../igstkTrackerController.cxx
    ../igstkTrackerConfigurationXMLFileReaderBase.cxx
    ../igstkSerialCommunicatingTrackerConfigurationXMLFileReader.cxx
    ../igstkPolarisWirelessConfigurationXMLFileReader.cxx
    ../igstkPolarisVicraConfigurationXMLFileReader.cxx
    ../igstkPolarisSpectraConfigurationXMLFileReader.cxx
    ../igstkPolarisHybridConfigurationXMLFileReader.cxx
    ../igstkAuroraConfigurationXMLFileReader.cxx
    ../igstkMicronConfigurationXMLFileReader.cxx
    ../igstkTrackerConfigurationFileReader.cxx
    main.cxx
)
SET(HDRS
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkPrecomputedTransformData.h
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkTransformXMLFileReaderBase.h
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkRigidTransformXMLFileReader.h
    ../../igstkTrackerConfiguration.h
    ../../igstkAuroraTrackerConfiguration.h
    ../../igstkMicronTrackerConfiguration.h
    ../../igstkPolarisTrackerConfiguration.h
    ../../igstkTrackerController.h
    ../igstkTrackerConfigurationXMLFileReaderBase.h
    ../igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h
    ../igstkPolarisWirelessConfigurationXMLFileReader.h
    ../igstkPolarisVicraConfigurationXMLFileReader.h
    ../igstkPolarisSpectraConfigurationXMLFileReader.h
    ../igstkPolarisHybridConfigurationXMLFileReader.h
    ../igstkAuroraConfigurationXMLFileReader.h
    ../igstkMicronConfigurationXMLFileReader.h
    ../igstkTrackerConfigurationFileReader.h
)

#
#The directories containing all of the header files
#
INCLUDE_DIRECTORIES(
..
../..
${PRECOMPUTED_TRANSFORM_READER_DIR}
${IGSTKSandbox_INCLUDE_DIR}
${IGSTKSandbox_DIR}
)

#
# Link to the following libraries
#
LINK_LIBRARIES (
  IGSTKSandbox
)


ADD_EXECUTABLE(TrackerConfigurationExample ${SRCS} ${HDRS})

TARGET_LINK_LIBRARIES(TrackerConfigurationExample ${LINK_LIBRARIES})
