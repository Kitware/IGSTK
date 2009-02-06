PROJECT(NeedleBiopsy)
  
SET(NeedleBiopsy_GUI_SRCS
  NeedleBiopsyGUI.fl
  PolarisTrackerConfigurationGUI.fl
  AuroraTrackerConfigurationGUI.fl
  MicronTrackerConfigurationGUI.fl
  )
  
SET(NeedleBiopsy_SRCS
  main.cxx
  NeedleBiopsy.h
  NeedleBiopsy.cxx
  igstkQuadrantViews.h
  igstkQuadrantViews.cxx
  igstkTrackerInitializer.h
  igstkTrackerInitializer.cxx
  igstkTrackerConfiguration.h
  igstkTrackerConfiguration.cxx
  igstkTrackerConfigurationGUIBase.h
  igstkTrackerConfigurationGUIBase.cxx
  igstkTreatmentPlan.h
  igstkTreatmentPlan.cxx
  igstkTreatmentPlanIO.h
  igstkTreatmentPlanIO.cxx
  igstkMicronTrackerConfigurationIO.h
  igstkMicronTrackerConfigurationIO.cxx
  )
  
IF(IGSTK_USE_MicronTracker)
  SET(NeedleBiopsy_SRCS ${NeedleBiopsy_SRCS}
  # MicronTracker related classes
  )
ENDIF(IGSTK_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${NeedleBiopsy_SOURCE_DIR}
  ${NeedleBiopsy_BINARY_DIR}
  )

FLTK_WRAP_UI(NeedleBiopsy ${NeedleBiopsy_GUI_SRCS})
ADD_EXECUTABLE(NeedleBiopsy ${NeedleBiopsy_FLTK_UI_SRCS} ${NeedleBiopsy_SRCS})
TARGET_LINK_LIBRARIES(NeedleBiopsy IGSTK)


