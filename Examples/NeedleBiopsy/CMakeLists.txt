PROJECT(NeedleBiopsy)
  
SET(NeedleBiopsy_GUI_SRCS
  NeedleBiopsyGUI.fl
  )
  
SET(NeedleBiopsy_SRCS
  main.cxx
  NeedleBiopsy.h
  NeedleBiopsy.cxx
  igstkQuadrantViews.h
  igstkQuadrantViews.cxx
  TrackerInitializer.h
  TrackerInitializer.cxx
  TrackerConfiguration.h
  TrackerConfiguration.cxx
  TrackerConfigurationGUI.h
  TrackerConfigurationGUI.cxx
  )
  
IF(IGSTKSandbox_USE_MicronTracker)
  SET(NeedleBiopsy_SRCS ${NeedleBiopsy_SRCS}
  # MicronTracker related classes
  )
ENDIF(IGSTKSandbox_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${NeedleBiopsy_SOURCE_DIR}
  ${NeedleBiopsy_BINARY_DIR}
  )

FLTK_WRAP_UI(NeedleBiopsy ${NeedleBiopsy_GUI_SRCS})
ADD_EXECUTABLE(NeedleBiopsy ${NeedleBiopsy_FLTK_UI_SRCS} ${NeedleBiopsy_SRCS})
TARGET_LINK_LIBRARIES(NeedleBiopsy IGSTKSandbox)


