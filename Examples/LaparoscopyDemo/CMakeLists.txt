PROJECT(LaparoscopyDemo)
  
SET(LaparoscopyDemo_GUI_SRCS
  LaparoscopyDemoGUI.fl
  PolarisTrackerConfigurationGUI.fl
  AuroraTrackerConfigurationGUI.fl
  MicronTrackerConfigurationGUI.fl
  )
  
SET(LaparoscopyDemo_SRCS
  main.cxx
  LaparoscopyDemo.h
  LaparoscopyDemo.cxx
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
  igstkImageSliceRepresentation.h
  igstkImageSliceRepresentation.txx
  )
  
IF(IGSTKSandbox_USE_MicronTracker)
  SET(LaparoscopyDemo_SRCS ${LaparoscopyDemo_SRCS}
  # MicronTracker related classes
  )
ENDIF(IGSTKSandbox_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${LaparoscopyDemo_SOURCE_DIR}
  ${LaparoscopyDemo_BINARY_DIR}
  )

FLTK_WRAP_UI(LaparoscopyDemo ${LaparoscopyDemo_GUI_SRCS})
ADD_EXECUTABLE(LaparoscopyDemo ${LaparoscopyDemo_FLTK_UI_SRCS} ${LaparoscopyDemo_SRCS})
TARGET_LINK_LIBRARIES(LaparoscopyDemo IGSTKSandbox)


