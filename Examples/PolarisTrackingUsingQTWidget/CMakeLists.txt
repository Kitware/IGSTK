PROJECT(PolarisTrackingUsingQTWidget)
  
SET(PolarisTrackingUsingQTWidget_GUI_SRCS
  PolarisTrackingUsingQTWidgetGUI.ui
  )

INCLUDE_DIRECTORIES(
  ${PolarisTrackingUsingQTWidget_SOURCE_DIR}
  ${PolarisTrackingUsingQTWidget_BINARY_DIR}
)

  
QT4_WRAP_UI(PolarisTrackingUsingQTWidget_QT_UI_SRCS ${PolarisTrackingUsingQTWidget_GUI_SRCS})

SET(PolarisTrackingUsingQTWidget_SRCS
  PolarisTrackingUsingQTWidget.cxx
  PolarisTrackingUsingQTWidgetGUI.cxx
  )

QT4_AUTOMOC(${PolarisTrackingUsingQTWidget_SRCS})

ADD_EXECUTABLE(PolarisTrackingUsingQTWidget ${PolarisTrackingUsingQTWidget_QT_UI_SRCS} ${PolarisTrackingUsingQTWidget_SRCS})

TARGET_LINK_LIBRARIES(PolarisTrackingUsingQTWidget IGSTKSandbox
${QT_LIBRARIES})




