PROJECT(WorkingVolumeTester)

INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ConfigurationFileReaders
) 
 
SET(WorkingVolumeTester_GUI_SRCS
    WorkingVolumeTesterGUI.fl
  )
  
SET(WorkingVolumeTester_SRCS
  main.cxx
  WorkingVolumeTester.h
  WorkingVolumeTester.cxx
  WorkingVolumeTesterQuadrantViews.h
  WorkingVolumeTesterQuadrantViews.cxx

  )
  
IF(WorkingVolumeTester_USE_MicronTracker)
  SET(WorkingVolumeTester_SRCS ${WorkingVolumeTester_SRCS}
  # MicronTracker related classes
  )
ENDIF(WorkingVolumeTester_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${WorkingVolumeTester_SOURCE_DIR}
  ${WorkingVolumeTester_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  )

FLTK_WRAP_UI(WorkingVolumeTester ${WorkingVolumeTester_GUI_SRCS})
ADD_EXECUTABLE(WorkingVolumeTester ${WorkingVolumeTester_FLTK_UI_SRCS} ${WorkingVolumeTester_SRCS})
TARGET_LINK_LIBRARIES(WorkingVolumeTester TrackerConfiguration CalibrationRegistrationLoading ConfigurationFileReaders IGSTKSandbox )
