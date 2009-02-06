PROJECT(TrackingVolumeViewer)

INCLUDE_DIRECTORIES (
  ${IGSTK_BINARY_DIR}
  ${IGSTK_BINARY_DIR}/Testing
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders
)

SET(TrackingVolumeViewer_GUI_SRCS
    TrackingVolumeViewerGUI.fl
  )

SET(TrackingVolumeViewer_SRCS
  main.cxx
  TrackingVolumeViewer.h
  TrackingVolumeViewer.cxx
  TrackingVolumeViewerQuadrantViews.h
  TrackingVolumeViewerQuadrantViews.cxx

  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkPrecomputedTransformData.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkTransformXMLFileReaderBase.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkRigidTransformXMLFileReader.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkTransformFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkTrackerConfigurationXMLFileReaderBase.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisWirelessConfigurationXMLFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisVicraConfigurationXMLFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisSpectraConfigurationXMLFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisHybridConfigurationXMLFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkAuroraConfigurationXMLFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkAscensionConfigurationXMLFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkMicronConfigurationXMLFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkTrackerConfigurationFileReader.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkTrackerConfiguration.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkPolarisTrackerConfiguration.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkAuroraTrackerConfiguration.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkMicronTrackerConfiguration.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkAscensionTrackerConfiguration.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkTrackerController.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkTrackerConfigurationXMLFileReaderBase.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisWirelessConfigurationXMLFileReader.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisVicraConfigurationXMLFileReader.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisSpectraConfigurationXMLFileReader.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisHybridConfigurationXMLFileReader.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkAuroraConfigurationXMLFileReader.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkAscensionConfigurationXMLFileReader.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkMicronConfigurationXMLFileReader.h

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkTrackerConfigurationFileReader.h
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkTrackerConfiguration.h
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkPolarisTrackerConfiguration.h
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkAuroraTrackerConfiguration.h
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkMicronTrackerConfiguration.h
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkAscensionTrackerConfiguration.h
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkTrackerController.h
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkPrecomputedTransformData.h
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkTransformXMLFileReaderBase.h
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkRigidTransformXMLFileReader.h
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkTransformFileReader.h
  )

IF(IGSTK_USE_MicronTracker)
  SET(TrackingVolumeViewer_SRCS ${TrackingVolumeViewer_SRCS}
  # MicronTracker related classes
  )
ENDIF(IGSTK_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${TrackingVolumeViewer_SOURCE_DIR}
  ${TrackingVolumeViewer_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration
  )

FLTK_WRAP_UI(TrackingVolumeViewer ${TrackingVolumeViewer_GUI_SRCS})
ADD_EXECUTABLE(TrackingVolumeViewer ${TrackingVolumeViewer_FLTK_UI_SRCS} ${TrackingVolumeViewer_SRCS})
TARGET_LINK_LIBRARIES(TrackingVolumeViewer IGSTK )
