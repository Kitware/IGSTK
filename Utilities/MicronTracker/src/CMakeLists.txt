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
  Vector.cpp
  Xform3D.cpp
  XPoints.cpp
  MTXPoint.cpp
  UtilityFunctions.cpp
  )

ADD_LIBRARY(igstkMTCu ${SOURCES})

IF(WIN32)
  SET(EXTRA_LIBS ${EXTRA_LIBS}  comctl32 wsock32 odbc32 odbccp32 )
ELSE(WIN32)
  SET(EXTRA_LIBS ${EXTRA_LIBS} raw1394 dc1394 )
ENDIF(WIN32)

TARGET_LINK_LIBRARIES( igstkMTCu MTC ${EXTRA_LIBS} )
