PROJECT(TrackerDataLogger)

SET(TRACKINGDATALOGGER_SOURCE_DIR
   ${IGSTK_SOURCE_DIR}/Examples/TrackerDataLogger
)

SET(TRACKINGDATALOGGER_BINARY_DIR
   ${IGSTK_BINARY_DIR}/Examples/TrackerDataLogger
)  

SET(PRECOMPUTED_TRANSFORM_READER_DIR
   ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
)

SET(TRACKER_CONFIGURATION_DIR
    ../TrackerConfiguration
)

SET(TRACKER_CONFIGURATION_READERS_DIR
    ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders
)


 INCLUDE_DIRECTORIES (
  ${IGSTK_SOURCE_DIR}
  ${IGSTK_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Source
  ${IGSTK_BINARY_DIR}/Source
  ${TRACKINGDATALOGGER_SOURCE_DIR}
  ${TRACKINGDATALOGGER_BINARY_DIR}
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

ADD_EXECUTABLE(TrackerDataLogger igstkTrackerDataLoggerMainCommandLine.cxx ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(TrackerDataLogger TrackerConfiguration TransformReaders IGSTK)

  
IF(IGSTK_USE_FLTK)
  SET(igstkTrackerDataLogger_GUI_SRCS
      igstkTrackerDataLoggerGUI.fl
    )
  FLTK_WRAP_UI(igstkTrackerDataLoggerGUI ${igstkTrackerDataLogger_GUI_SRCS})
  ADD_EXECUTABLE(igstkTrackerDataLoggerGUI igstkTrackerDataLoggerMainFLTK ${SRCS} ${HDRS} ${igstkTrackerDataLoggerGUI_FLTK_UI_SRCS})  
  TARGET_LINK_LIBRARIES(igstkTrackerDataLoggerGUI TrackerConfiguration TransformReaders IGSTK)
ENDIF(IGSTK_USE_FLTK)


