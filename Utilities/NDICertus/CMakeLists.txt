PROJECT(NDICertus)

SET( SOURCES
  certus_aux.c
  certus_aux.h
  datatypes.h
  ndhost.h
  ndopto.h
  ndpack.h
  ndtypes.h
  )

  INCLUDE_DIRECTORIES(
  ${NDICertus_SOURCE_DIR}
  )
  
ADD_LIBRARY(igstkNDICertus ${SOURCES})
