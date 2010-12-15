PROJECT(PETCTNeedleBiopsy)
  
SET(PETCTNeedleBiopsy_GUI_SRCS
  PETCTNeedleBiopsyGUI.fl
  PolarisTrackerConfigurationGUI.fl
  AuroraTrackerConfigurationGUI.fl
  MicronTrackerConfigurationGUI.fl
  )
  
SET(PETCTNeedleBiopsy_SRCS
  main.cxx
  PETCTNeedleBiopsy.h
  PETCTNeedleBiopsy.cxx
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
  igstkPolarisTrackerConfigurationIO.h
  igstkPolarisTrackerConfigurationIO.cxx
  igstkImageSliceRepresentation.h
  igstkImageSliceRepresentation.txx
  igstkImageSliceRepresentationPlus.h
  igstkImageSliceRepresentationPlus.txx
  )
  
IF(IGSTKSandbox_USE_MicronTracker)
  SET(PETCTNeedleBiopsy_SRCS ${PETCTNeedleBiopsy_SRCS}
  # MicronTracker related classes
  )
ENDIF(IGSTKSandbox_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${PETCTNeedleBiopsy_SOURCE_DIR}
  ${PETCTNeedleBiopsy_BINARY_DIR}
  ${PETCTNeedleBiopsy_SOURCE_DIR}/../PETDicomReader
  )

FLTK_WRAP_UI(PETCTNeedleBiopsy ${PETCTNeedleBiopsy_GUI_SRCS})
ADD_EXECUTABLE(PETCTNeedleBiopsy ${PETCTNeedleBiopsy_FLTK_UI_SRCS} ${PETCTNeedleBiopsy_SRCS})
TARGET_LINK_LIBRARIES(PETCTNeedleBiopsy IGSTKSandbox)


