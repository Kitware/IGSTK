PROJECT(Navigator)

INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/configurationFileReaders
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/Tools
) 
 
SET(Navigator_GUI_SRCS
    NavigatorGUI.fl
   )
  
SET(Navigator_SRCS
  main.cxx
  Navigator.h
  Navigator.cxx
  NavigatorQuadrantViews.h
  NavigatorQuadrantViews.cxx
)
  
IF(Navigator_USE_MicronTracker)
  SET(Navigator_SRCS ${Navigator_SRCS}
  # MicronTracker related classes
  )
ENDIF(Navigator_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${Navigator_SOURCE_DIR}
  ${Navigator_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  )

FLTK_WRAP_UI(Navigator ${Navigator_GUI_SRCS})
ADD_EXECUTABLE(Navigator ${Navigator_FLTK_UI_SRCS} ${Navigator_SRCS})
TARGET_LINK_LIBRARIES(Navigator TrackerConfiguration CalibrationRegistrationLoading configurationFileReaders Tools IGSTKSandbox )

