PROJECT(OIGTLinkTrackerBroadcasting)

SET(PRECOMPUTED_TRANSFORM_READER_DIR
    ../ApplicationsCommon/CalibrationRegistrationLoading
)

SET(TRACKER_CONFIGURATION_DIR
    ../ApplicationsCommon/TrackerConfiguration
)

SET(TRACKER_CONFIGURATION_READERS_DIR
    ../ApplicationsCommon/ConfigurationFileReaders
)

SET(OIGTLINK_CONFIGURATION_READERS_DIR
    ../ApplicationsCommon/OIGTLinkConfigurationReaders
)



#
# Source code for the project
#
SET(SRCS
    OIGTLinkTrackingBroadcaster.cxx
)
SET(HDRS
    OIGTLinkTrackingBroadcaster.h
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
${OIGTLINK_CONFIGURATION_READERS_DIR}
${PRECOMPUTED_TRANSFORM_READER_DIR}
${IGSTKSandbox_INCLUDE_DIR}
${OIGTLinkTrackerBroadcasting_SOURCE_DIR}
${OIGTLinkTrackerBroadcasting_BINARY_DIR}
)

IF(IGSTKSandbox_USE_FLTK)

  #
  # FLTK fluid GUI files for project
  #
  SET(OIGTLinkTrackerBroadcasting_GUI_SRCS
      OIGTLinkTrackingBroadcasterGUI.fl
    )

  FLTK_WRAP_UI(OIGTLinkTrackerBroadcastingUI ${OIGTLinkTrackerBroadcasting_GUI_SRCS})
  ADD_EXECUTABLE(OIGTLinkTrackerBroadcastingUI mainFLTK.cxx ${SRCS} ${HDRS} ${OIGTLinkTrackerBroadcastingUI_FLTK_UI_SRCS})  
  TARGET_LINK_LIBRARIES(OIGTLinkTrackerBroadcastingUI TrackerConfiguration ConfigurationFileReaders OIGTLinkConfigurationReaders IGSTKSandbox)
  
ENDIF(IGSTKSandbox_USE_FLTK)

ADD_EXECUTABLE(OIGTLinkTrackerBroadcasting mainCommandLine.cxx ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(OIGTLinkTrackerBroadcasting TrackerConfiguration ConfigurationFileReaders OIGTLinkConfigurationReaders IGSTKSandbox)

