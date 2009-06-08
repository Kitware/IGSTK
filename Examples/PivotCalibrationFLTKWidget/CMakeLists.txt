PROJECT(PivotCalibrationFLTKWidgetExample)

SET(PivotCalibrationFLTKWidgetExample_GUI_SRCS
  PivotCalibrationFLTKWidgetExampleUI.fl
  )

SET( PivotCalibrationFLTKWidgetExample_SRCS
  PivotCalibrationFLTKWidgetExample.cxx
  main.cxx
)

INCLUDE_DIRECTORIES(
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
  ${IGSTK_SOURCE_DIR}/Examples/TransformWriters
  ${PivotCalibrationFLTKWidgetExample_SOURCE_DIR}
  ${PivotCalibrationFLTKWidgetExample_BINARY_DIR}
  )

FLTK_WRAP_UI(PivotCalibrationFLTKWidgetExample ${PivotCalibrationFLTKWidgetExample_GUI_SRCS})

ADD_EXECUTABLE(PivotCalibrationFLTKWidgetExample ${PivotCalibrationFLTKWidgetExample_FLTK_UI_SRCS} ${PivotCalibrationFLTKWidgetExample_SRCS})
TARGET_LINK_LIBRARIES(PivotCalibrationFLTKWidgetExample TransformReaders TransformWriters IGSTK)
