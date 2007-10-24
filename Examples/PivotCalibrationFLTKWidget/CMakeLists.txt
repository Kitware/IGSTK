PROJECT(PivotCalibrationFLTKWidgetExample)

SET(PivotCalibrationFLTKWidgetExample_GUI_SRCS
  PivotCalibrationFLTKWidgetExampleUI.fl
  )

SET(PivotCalibrationFLTKWidgetExample_SRCS
  PivotCalibrationFLTKWidgetExample.cxx
  main.cxx
  )

SET(PivotCalibrationFLTKWidgetExample_HDRS
    PivotCalibrationFLTKWidgetExample.h
)

INCLUDE_DIRECTORIES(
  ${PivotCalibrationFLTKWidgetExample_SOURCE_DIR}
  ${PivotCalibrationFLTKWidgetExample_BINARY_DIR}
  )

FLTK_WRAP_UI(PivotCalibrationFLTKWidgetExample ${PivotCalibrationFLTKWidgetExample_GUI_SRCS})

ADD_EXECUTABLE(PivotCalibrationFLTKWidgetExample ${PivotCalibrationFLTKWidgetExample_FLTK_UI_SRCS} ${PivotCalibrationFLTKWidgetExample_SRCS})
TARGET_LINK_LIBRARIES(PivotCalibrationFLTKWidgetExample IGSTKSandbox)
