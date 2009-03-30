PROJECT(PivotCalibrationFLTKWidgetExample)

SET(PivotCalibrationFLTKWidgetExample_GUI_SRCS
  PivotCalibrationFLTKWidgetExampleUI.fl
  )

SET( PivotCalibrationFLTKWidgetExample_SRCS
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkPrecomputedTransformData.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/TransformWriters/igstkTransformXMLFileWriterBase.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/TransformWriters/igstkRigidTransformXMLFileWriter.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/TransformWriters/igstkTransformFileWriter.cxx
  PivotCalibrationFLTKWidgetExample.cxx
  main.cxx
)

SET( PivotCalibrationFLTKWidgetExample_HDRS
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkPrecomputedTransformData.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/TransformWriters/igstkTransformXMLFileWriterBase.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/TransformWriters/igstkRigidTransformXMLFileWriter.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/TransformWriters/igstkTransformFileWriter.h
  PivotCalibrationFLTKWidgetExample.h
)


INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading
  ${IGSTKSandbox_SOURCE_DIR}/Examples/TransformWriters
  ${PivotCalibrationFLTKWidgetExample_SOURCE_DIR}
  ${PivotCalibrationFLTKWidgetExample_BINARY_DIR}
  )

FLTK_WRAP_UI(PivotCalibrationFLTKWidgetExample ${PivotCalibrationFLTKWidgetExample_GUI_SRCS})

ADD_EXECUTABLE(PivotCalibrationFLTKWidgetExample ${PivotCalibrationFLTKWidgetExample_FLTK_UI_SRCS} ${PivotCalibrationFLTKWidgetExample_SRCS})
TARGET_LINK_LIBRARIES(PivotCalibrationFLTKWidgetExample IGSTKSandbox)
