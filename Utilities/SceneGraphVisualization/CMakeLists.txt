PROJECT(SceneGraphVisualization)

SET(SceneGraphVisualization_SRC
    igstkSceneGraph.cxx
    igstkSceneGraph.h
    igstkSceneGraphNode.cxx
    igstkSceneGraphNode.h
    igstkSceneGraphObserver.h
)

IF (IGSTK_USE_FLTK)
  SET(SceneGraphVisualization_SRC
      ${SceneGraphVisualization_SRC}
      igstkSceneGraphUI.cxx
      igstkSceneGraphUI.h
      )
ENDIF (IGSTK_USE_FLTK)

INCLUDE_DIRECTORIES(
  ${SceneGraphVisualization_SOURCE_DIR}
  ${SceneGraphVisualization_BINARY_DIR}
  )
  
ADD_LIBRARY(SceneGraphVisualization ${SceneGraphVisualization_SRC} )

# Apply user-defined properties to the library target.
IF(IGSTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(SceneGraphVisualization PROPERTIES
     ${IGSTK_LIBRARY_PROPERTIES}
  )
ENDIF(IGSTK_LIBRARY_PROPERTIES)

INSTALL(TARGETS SceneGraphVisualization
   DESTINATION ${IGSTK_INSTALL_LIB_DIR}
)

FILE(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")

INSTALL(FILES ${__files1} ${__files2}
   DESTINATION ${IGSTK_INSTALL_INCLUDE_DIR}
)

