# Add proper includes
INCLUDE_DIRECTORIES(
  ${MTC_INCLUDE_DIRECTORY}
)

SET( SOURCES
  Cameras.cpp
  Collection.cpp
  Facet.cpp 
  MCamera.cpp
  Marker.cpp
  Markers.cpp
  Persistence.cpp
  UtilityFunctions.cpp
  Vector.cpp
  Xform3D.cpp
  )

ADD_LIBRARY(igstkMTCu ${SOURCES})
TARGET_LINK_LIBRARIES( igstkMTCu MTC )
