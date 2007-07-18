PROJECT(OneViewAndTrackingUsingQTWidget)
  
SET(OneViewAndTrackingUsingQTWidget_GUI_SRCS
  OneViewAndTrackingUsingQTWidgetGUI.ui
  )

INCLUDE_DIRECTORIES(
  ${OneViewAndTrackingUsingQTWidget_SOURCE_DIR}
  ${OneViewAndTrackingUsingQTWidget_BINARY_DIR}
)

  
QT4_WRAP_UI(OneViewAndTrackingUsingQTWidget_QT_UI_SRCS ${OneViewAndTrackingUsingQTWidget_GUI_SRCS})

SET(OneViewAndTrackingUsingQTWidget_SRCS
  OneViewAndTrackingUsingQTWidget.cxx
  OneViewAndTrackingUsingQTWidgetGUI.cxx
  )

QT4_AUTOMOC(${OneViewAndTrackingUsingQTWidget_SRCS})

ADD_EXECUTABLE(OneViewAndTrackingUsingQTWidget ${OneViewAndTrackingUsingQTWidget_QT_UI_SRCS} ${OneViewAndTrackingUsingQTWidget_SRCS})

TARGET_LINK_LIBRARIES(OneViewAndTrackingUsingQTWidget IGSTKSandbox
${QT_LIBRARIES})



