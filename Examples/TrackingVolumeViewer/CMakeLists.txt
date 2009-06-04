PROJECT(TrackingVolumeViewer)

INCLUDE_DIRECTORIES (
  ${IGSTK_BINARY_DIR}
  ${IGSTK_BINARY_DIR}/Testing
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders
  ${TrackingVolumeViewer_SOURCE_DIR}
  ${TrackingVolumeViewer_BINARY_DIR}
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
  )

IF(IGSTK_USE_MicronTracker)
  SET(TrackingVolumeViewer_SRCS ${TrackingVolumeViewer_SRCS}
  # MicronTracker related classes
  )
ENDIF(IGSTK_USE_MicronTracker)

FLTK_WRAP_UI(TrackingVolumeViewer ${TrackingVolumeViewer_GUI_SRCS})
ADD_EXECUTABLE(TrackingVolumeViewer ${TrackingVolumeViewer_FLTK_UI_SRCS} ${TrackingVolumeViewer_SRCS})
TARGET_LINK_LIBRARIES(TrackingVolumeViewer TrackerConfiguration TransformReaders IGSTK )
