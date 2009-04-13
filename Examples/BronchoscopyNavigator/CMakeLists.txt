PROJECT(BronchoscopyNavigator)

INCLUDE_DIRECTORIES (
${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ConfigurationFileReaders
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/Tools

) 
 
SET(BronchoscopyNavigator_GUI_SRCS
    BronchoscopyNavigatorGUI.fl
   )
  
SET(BronchoscopyNavigator_SRCS
  main.cxx
  BronchoscopyNavigator.h
  BronchoscopyNavigator.cxx
  BronchoscopyNavigatorQuadrantViews.h
  BronchoscopyNavigatorQuadrantViews.cxx

  )
  
IF(BronchoscopyNavigator_USE_MicronTracker)
  SET(BronchoscopyNavigator_SRCS ${BronchoscopyNavigator_SRCS}
  # MicronTracker related classes
  )
ENDIF(BronchoscopyNavigator_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${BronchoscopyNavigator_SOURCE_DIR}
  ${BronchoscopyNavigator_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  )

FLTK_WRAP_UI(BronchoscopyNavigator ${BronchoscopyNavigator_GUI_SRCS})
ADD_EXECUTABLE(BronchoscopyNavigator ${BronchoscopyNavigator_FLTK_UI_SRCS} ${BronchoscopyNavigator_SRCS})
TARGET_LINK_LIBRARIES(BronchoscopyNavigator TrackerConfiguration CalibrationRegistrationLoading ConfigurationFileReaders Tools IGSTKSandbox )

