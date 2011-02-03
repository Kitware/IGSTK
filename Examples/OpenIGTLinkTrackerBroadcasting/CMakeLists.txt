PROJECT(OpenIGTLinkTrackerBroadcasting)

SET(PRECOMPUTED_TRANSFORM_READER_DIR
    ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
)

SET(TRACKER_CONFIGURATION_DIR
    ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration
)

SET(TRACKER_CONFIGURATION_READERS_DIR
    ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders
)

SET(OpenIGTLINK_CONFIGURATION_READERS_DIR
    ${IGSTK_SOURCE_DIR}/Examples/OpenIGTLinkConfigurationReaders
)



#
# Source code for the project
#
SET(SRCS
    OpenIGTLinkTrackingBroadcaster.cxx
)
SET(HDRS
    OpenIGTLinkTrackingBroadcaster.h
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
${OpenIGTLINK_CONFIGURATION_READERS_DIR}
${PRECOMPUTED_TRANSFORM_READER_DIR}
${IGSTK_INCLUDE_DIR}
${OpenIGTLinkTrackerBroadcasting_SOURCE_DIR}
${OpenIGTLinkTrackerBroadcasting_BINARY_DIR}
)

IF(IGSTK_USE_FLTK)

  #
  # FLTK fluid GUI files for project
  #
  SET(OpenIGTLinkTrackerBroadcasting_GUI_SRCS
      OpenIGTLinkTrackingBroadcasterGUI.fl
    )

  FLTK_WRAP_UI(OpenIGTLinkTrackerBroadcastingUI ${OpenIGTLinkTrackerBroadcasting_GUI_SRCS})
  ADD_EXECUTABLE(OpenIGTLinkTrackerBroadcastingUI OpenIGTLinkTrackingBroadcasterMainFLTK.cxx ${SRCS} ${HDRS} ${OpenIGTLinkTrackerBroadcastingUI_FLTK_UI_SRCS})  
  TARGET_LINK_LIBRARIES(OpenIGTLinkTrackerBroadcastingUI TrackerConfiguration TransformReaders OpenIGTLinkConfigurationReaders IGSTK)
  
ENDIF(IGSTK_USE_FLTK)

ADD_EXECUTABLE(OpenIGTLinkTrackerBroadcasting OpenIGTLinkTrackingBroadcasterMainCommandLine.cxx ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(OpenIGTLinkTrackerBroadcasting TrackerConfiguration TransformReaders OpenIGTLinkConfigurationReaders IGSTK)

