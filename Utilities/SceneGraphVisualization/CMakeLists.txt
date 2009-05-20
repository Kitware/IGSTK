PROJECT(SceneGraphVisualization)

SET(SceneGraphVisualization_SRC
    igstkSceneGraph.cxx
    igstkSceneGraph.h
    igstkSceneGraphNode.cxx
    igstkSceneGraphNode.h
    igstkSceneGraphObserver.h
)

IF (IGSTKSandbox_USE_FLTK)
  SET(SceneGraphVisualization_SRC
      ${SceneGraphVisualization_SRC}
      igstkSceneGraphUI.cxx
      igstkSceneGraphUI.h
      )
ENDIF (IGSTKSandbox_USE_FLTK)

INCLUDE_DIRECTORIES(
  ${SceneGraphVisualization_SOURCE_DIR}
  ${SceneGraphVisualization_BINARY_DIR}
  )
  
ADD_LIBRARY(SceneGraphVisualization ${SceneGraphVisualization_SRC} )
TARGET_LINK_LIBRARIES(SceneGraphVisualization IGSTKSandbox)
