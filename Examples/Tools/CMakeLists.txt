PROJECT(Tools)  
  
SET(Tools_SRCS
  igstkPathPlanIO.cxx
  igstkPathPlanIO.h
  igstkPathPlan.cxx
  igstkPathPlan.h
  igstkFiducialsPlanIO.cxx
  igstkFiducialsPlanIO.h
  igstkFiducialsPlan.cxx
  igstkFiducialsPlan.h
)
  
INCLUDE_DIRECTORIES(
  ${Tools_SOURCE_DIR}
  ${Tools_BINARY_DIR}
  )

ADD_LIBRARY(Tools ${Tools_SRCS})
TARGET_LINK_LIBRARIES(Tools IGSTK)


