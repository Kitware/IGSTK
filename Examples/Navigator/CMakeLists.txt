PROJECT(Navigator)

INCLUDE_DIRECTORIES (
  ${IGSTK_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders
  ${IGSTK_SOURCE_DIR}/Examples/Tools
) 
 
SET(Navigator_GUI_SRCS
    NavigatorGUI.fl
   )
  
SET(Navigator_SRCS
  main.cxx
  Navigator.h
  Navigator.cxx
  NavigatorQuadrantViews.h
  NavigatorQuadrantViews.cxx
)
  
IF(IGSTK_USE_MicronTracker)
  SET(Navigator_SRCS ${Navigator_SRCS}
  # MicronTracker related classes
  )
ENDIF(IGSTK_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${Navigator_SOURCE_DIR}
  ${Navigator_BINARY_DIR}
  )

FLTK_WRAP_UI(Navigator ${Navigator_GUI_SRCS})
ADD_EXECUTABLE(Navigator ${Navigator_FLTK_UI_SRCS} ${Navigator_SRCS})
TARGET_LINK_LIBRARIES(Navigator TrackerConfiguration TransformReaders Tools IGSTK )

