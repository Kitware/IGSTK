PROJECT(AtamaiTraking)

INCLUDE_REGULAR_EXPRESSION("^(vtk).*$")

SET(KIT AtamaiTracking)
SET(KIT ATAMAITRACKING)
SET(KIT_LIBS vtkHybrid)

SET ( Kit_SRCS
  vtkTracker.cxx
  vtkTrackerTool.cxx
  vtkTrackerBuffer.cxx
  vtkFrameToTimeConverter.cxx
  )

INCLUDE_DIRECTORIES(
  ${AtamaiTracking_SOURCE_DIR}
)

SET_SOURCE_FILES_PROPERTIES(
  vtkTracker
  ABSTRACT
  )


IF (VTK_USE_POLARIS)
  SET ( Kit_SRCS ${Kit_SRCS}
        vtkPOLARISTracker.cxx
  vtkNDITracker.cxx)
ENDIF (VTK_USE_POLARIS)


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

# Allow the user to customize their build with some local options
#
SET(LOCALUSERMACRODEFINED 0)
INCLUDE (${VTK_BINARY_DIR}/${KIT}/LocalUserOptions.cmake OPTIONAL)
INCLUDE (${VTK_SOURCE_DIR}/${KIT}/LocalUserOptions.cmake OPTIONAL)

TARGET_LINK_LIBRARIES(vtk${KIT} ${KIT_LIBS})

INSTALL_TARGETS(/lib/vtk vtk${KIT})
INSTALL_FILES(/include/vtk .h ${Kit_SRCS})

# End of common section
# -----------------------------------------------------------------
IF (VTK_USE_POLARIS)
  TARGET_LINK_LIBRARIES (vtkAtamaiTracking vtkndicapi)
ENDIF (VTK_USE_POLARIS)

