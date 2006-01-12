PROJECT(ATAMAITRACKING)

INCLUDE_REGULAR_EXPRESSION("^(vtk).*$")

SET(KIT AtamaiTracking)
SET(KIT_LIBS vtkHybrid ndicapi flock)

SET ( Kit_SRCS
  vtkTracker.cxx
  vtkFlockTracker.cxx
  vtkPOLARISTracker.cxx
  vtkNDITracker.cxx
  vtkTrackerTool.cxx
  vtkTrackerBuffer.cxx
  vtkFrameToTimeConverter.cxx
  )

SET_SOURCE_FILES_PROPERTIES(
  vtkTracker
  ABSTRACT
  )

INCLUDE_DIRECTORIES(
  ${ATAMAITRACKING_SOURCE_DIR}
  ${NDICAPI_SOURCE_DIR}
  ${FLOCK_SOURCE_DIR}
)

SET(Kit_EXTRA_SRCS)
SET(Kit_EXTRA_CMDS)

# -----------------------------------------------------------------
# Start of common section

## Setup vtkInstantiator registration for this library's classes.
#VTK_MAKE_INSTANTIATOR2(vtk${KIT}Instantiator KitInstantiator_SRCS
#                       ${Kit_SRCS}
#                       EXPORT_MACRO VTK_${UKIT}_EXPORT
#                       HEADER_LOCATION ${VTK_BINARY_DIR})

#ADD_LIBRARY(vtk${KIT} ${Kit_SRCS} ${Kit_EXTRA_SRCS} ${KitInstantiator_SRCS})
ADD_LIBRARY(vtk${KIT} ${Kit_SRCS} ${Kit_EXTRA_SRCS})

#IF(VTK_APPLE_SHARED_FLAGS_NEEDED)
#  SET_TARGET_PROPERTIES(vtk${KIT} PROPERTIES LINK_FLAGS "-init _oft_init${KIT}")
#ENDIF(VTK_APPLE_SHARED_FLAGS_NEEDED)

TARGET_LINK_LIBRARIES(vtk${KIT} ${KIT_LIBS})

# End of common section
# -----------------------------------------------------------------

