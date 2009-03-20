PROJECT(OIGTLinkTrackerBroadcasting)

SET(PRECOMPUTED_TRANSFORM_READER_DIR
    ../ApplicationsCommon/CalibrationRegistrationLoading
)

SET(TRACKER_CONFIGURATION_DIR
    ../ApplicationsCommon/TrackerConfiguration
)

SET(TRACKER_CONFIGURATION_READERS_DIR
    ../ApplicationsCommon/TrackerConfiguration/configurationFileReaders
)

SET(OIGTLINK_CONFIGURATION_READERS_DIR
    ../ApplicationsCommon/TrackerConfiguration/configurationFileReaders/oigtLinkConfigurationReaders
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
    ${TRACKER_CONFIGURATION_DIR}/igstkAscensionTrackerConfiguration.cxx
    ${TRACKER_CONFIGURATION_DIR}/igstkTrackerController.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkTrackerConfigurationXMLFileReaderBase.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisWirelessConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisVicraConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisSpectraConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisHybridConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkAuroraConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkMicronConfigurationXMLFileReader.cxx
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkAscensionConfigurationXMLFileReader.cxx
    ${OIGTLINK_CONFIGURATION_READERS_DIR}/igstkOIGTLinkTrackerConfigurationFileReader.cxx
    ${OIGTLINK_CONFIGURATION_READERS_DIR}/igstkOIGTLinkTrackerConfigurationXMLFileReader.cxx
    OIGTLinkTrackingBroadcaster.cxx
)
SET(HDRS
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkPrecomputedTransformData.h
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkTransformXMLFileReaderBase.h
    ${PRECOMPUTED_TRANSFORM_READER_DIR}/igstkRigidTransformXMLFileReader.h
    ${TRACKER_CONFIGURATION_DIR}/igstkTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkAuroraTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkMicronTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkPolarisTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkAscensionTrackerConfiguration.h
    ${TRACKER_CONFIGURATION_DIR}/igstkTrackerController.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkTrackerConfigurationXMLFileReaderBase.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisWirelessConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisVicraConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisSpectraConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkPolarisHybridConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkAuroraConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkMicronConfigurationXMLFileReader.h
    ${TRACKER_CONFIGURATION_READERS_DIR}/igstkAscensionConfigurationXMLFileReader.h
    ${OIGTLINK_CONFIGURATION_READERS_DIR}/igstkOIGTLinkTrackerConfigurationFileReader.h
    ${OIGTLINK_CONFIGURATION_READERS_DIR}/igstkOIGTLinkTrackerConfigurationXMLFileReader.h
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
  TARGET_LINK_LIBRARIES(OIGTLinkTrackerBroadcastingUI IGSTKSandbox)
  
ENDIF(IGSTKSandbox_USE_FLTK)

ADD_EXECUTABLE(OIGTLinkTrackerBroadcasting mainCommandLine.cxx ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(OIGTLinkTrackerBroadcasting IGSTKSandbox)

