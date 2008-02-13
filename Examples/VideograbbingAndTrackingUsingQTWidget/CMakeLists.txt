PROJECT(VideograbbingAndTrackingUsingQTWidget)

INCLUDE_DIRECTORIES(
  ${VideograbbingAndTrackingUsingQTWidget_SOURCE_DIR}
  ${VideograbbingAndTrackingUsingQTWidget_BINARY_DIR}
)
SET (SOURCES
    VideograbbingAndTrackingUsingQTWidget.cxx
    mainwindow.cxx
    igstkVideoSpatialObject.cxx
    igstkVideoObjectRepresentation.cxx
)

QT4_WRAP_CPP( MOC_SRCS mainwindow.h)


ADD_EXECUTABLE (VideograbbingAndTrackingUsingQTWidget ${SOURCES} ${MOC_SRCS})
TARGET_LINK_LIBRARIES ( VideograbbingAndTrackingUsingQTWidget IGSTKSandbox 
  ${QT_LIBRARIES}
)

