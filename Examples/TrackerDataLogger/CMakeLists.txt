PROJECT(TrackerDataLogger)

SET( IGSTK_SOURCE_DIR
   ${IGSTKSandbox_SOURCE_DIR}/../IGSTK
)  
  
SET(TRACKINGDATALOGGER_DIR
   ${IGSTK_SOURCE_DIR}/../IGSTK/Examples/TrackerDataLogger
)

SET(PRECOMPUTED_TRANSFORM_READER_DIR
    ../ApplicationsCommon/CalibrationRegistrationLoading
)

SET(TRACKER_CONFIGURATION_DIR
    ../ApplicationsCommon/TrackerConfiguration
)

SET(TRACKER_CONFIGURATION_READERS_DIR
    ../ApplicationsCommon/ConfigurationFileReaders
)


 INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_SOURCE_DIR}
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/Source
  ${TRACKINGDATALOGGER_DIR}
  ${TRACKER_CONFIGURATION_DIR}
  ${TRACKER_CONFIGURATION_READER_DIR}
  ${PRECOMPUTED_TRANSFORM_READER_DIR}
  )

#
# Source code for the project
#
SET(SRCS
    igstkTrackerDataLogger.cxx
    igstkTrackerDataLoggerConfigurationXMLFileReader.cxx
    igstkTrackerDataLoggerConfigurationFileReader.cxx
)
SET(HDRS
    igstkTrackerDataLogger.h
    igstkTrackerDataLoggerConfigurationXMLFileReader.h
    igstkTrackerDataLoggerConfigurationFileReader.h
)

#
# Include directories.
# Project's binary directory is part of the includes because cmake runs the fluid
# "compiler" on the *.fl GUI description files to generate
# *.cxx, *.h files which are written into the binary directory.
#

INCLUDE_DIRECTORIES(
${TRACKER_CONFIGURATION_DIR}
${TRACKER_CONFIGURATION_READERS_DIR}
${PRECOMPUTED_TRANSFORM_READER_DIR}
${IGSTK_INCLUDE_DIR}
)

IF(IGSTK_USE_FLTK)

  #
  # FLTK fluid GUI files for project
  #
  SET(igstkTrackerDataLogger_GUI_SRCS
      igstkTrackerDataLoggerGUI.fl
    )

  FLTK_WRAP_UI(igstkTrackerDataLoggerUI ${igstkTrackerDataLogger_GUI_SRCS})
  ADD_EXECUTABLE(igstkTrackerDataLoggerUI igstkTrackerDataLoggerMainFLTK.cxx ${SRCS} ${HDRS} ${igstkTrackerDataLoggerUI_FLTK_UI_SRCS})  
  TARGET_LINK_LIBRARIES(igstkTrackerDataLoggerUI TrackerConfiguration ConfigurationFileReaders IGSTKSandbox)
  
ENDIF(IGSTK_USE_FLTK)

ADD_EXECUTABLE(TrackerDataLogger igstkTrackerDataLoggerMainCommandLine.cxx ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(TrackerDataLogger TrackerConfiguration ConfigurationFileReaders IGSTKSandbox)


