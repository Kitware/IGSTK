PROJECT(PivotCalibrationFLTKWidgetExampleNew)

SET(PivotCalibrationFLTKWidgetExampleNew_GUI_SRCS
  PivotCalibrationFLTKWidgetExampleNewUI.fl
  PolarisTrackerConfigurationGUI.fl
  AuroraTrackerConfigurationGUI.fl
  MicronTrackerConfigurationGUI.fl
  FlockOfBirdsTrackerConfigurationGUI.fl
  )

SET(PivotCalibrationFLTKWidgetExampleNew_SRCS
  PivotCalibrationFLTKWidgetExampleNew.cxx
  igstkTrackerInitializerNew.cxx
  igstkTrackerConfigurationNew.cxx
  igstkTrackerConfigurationGUIBase.cxx
  main.cxx
  )

SET(PivotCalibrationFLTKWidgetExampleNew_HDRS
    PivotCalibrationFLTKWidgetExampleNew.h
    igstkTrackerInitializerNew.h
    igstkTrackerConfigurationNew.h
    igstkTrackerConfigurationGUIBase.h
)

INCLUDE_DIRECTORIES(
  ${PivotCalibrationFLTKWidgetExampleNew_SOURCE_DIR}
  ${PivotCalibrationFLTKWidgetExampleNew_BINARY_DIR}
  )

FLTK_WRAP_UI(PivotCalibrationFLTKWidgetExampleNew ${PivotCalibrationFLTKWidgetExampleNew_GUI_SRCS})

ADD_EXECUTABLE(PivotCalibrationFLTKWidgetExampleNew ${PivotCalibrationFLTKWidgetExampleNew_FLTK_UI_SRCS} ${PivotCalibrationFLTKWidgetExampleNew_SRCS})
TARGET_LINK_LIBRARIES(PivotCalibrationFLTKWidgetExampleNew IGSTKSandbox)
