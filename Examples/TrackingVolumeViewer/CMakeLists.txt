PROJECT(TrackingVolumeViewer)

INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ConfigurationFileReaders
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
  
IF(TrackingVolumeViewer_USE_MicronTracker)
  SET(TrackingVolumeViewer_SRCS ${TrackingVolumeViewer_SRCS}
  # MicronTracker related classes
  )
ENDIF(TrackingVolumeViewer_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${TrackingVolumeViewer_SOURCE_DIR}
  ${TrackingVolumeViewer_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  )

FLTK_WRAP_UI(TrackingVolumeViewer ${TrackingVolumeViewer_GUI_SRCS})
ADD_EXECUTABLE(TrackingVolumeViewer ${TrackingVolumeViewer_FLTK_UI_SRCS} ${TrackingVolumeViewer_SRCS})
TARGET_LINK_LIBRARIES(TrackingVolumeViewer TrackerConfiguration CalibrationRegistrationLoading ConfigurationFileReaders IGSTKSandbox )
