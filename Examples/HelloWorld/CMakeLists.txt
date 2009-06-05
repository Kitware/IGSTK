PROJECT(HelloWorld)
  
SET(HelloWorld_GUI_SRCS
    HelloWorldGUI.fl
    )
  
SET(HelloWorld_SRCS
   HelloWorld.cxx
   )

INCLUDE_DIRECTORIES(
  ${HelloWorld_SOURCE_DIR}
  ${HelloWorld_BINARY_DIR}
  )

FLTK_WRAP_UI(HelloWorld ${HelloWorld_GUI_SRCS})
ADD_EXECUTABLE(HelloWorld ${HelloWorld_FLTK_UI_SRCS} ${HelloWorld_SRCS})

IF(IGSTK_USE_SceneGraphVisualization)
  SET(LibrariesToLink SceneGraphVisualization)
ENDIF(IGSTK_USE_SceneGraphVisualization)

TARGET_LINK_LIBRARIES(HelloWorld ${LibrariesToLink} IGSTK)

