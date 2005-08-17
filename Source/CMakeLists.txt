#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkAuroraTracker.h
  igstkAuroraTrackerTool.h
  igstkPolarisTracker.h
  igstkPolarisTrackerTool.h
  igstkCTImageReader.h
  igstkImageSpatialObject.h
  igstkImageSpatialObjectRepresentation.h
  igstkImageReader.h
  igstkNDICommandInterpreter.h
  igstkMouseTracker.h
  igstkTubeObject.h
  igstkTubeObjectRepresentation.h
  igstkMeshObject.h
  igstkMeshObjectRepresentation.h
)


# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkAuroraTracker.cxx
  igstkAuroraTrackerTool.cxx
  igstkPolarisTracker.cxx
  igstkPolarisTrackerTool.cxx
  igstkCTImageReader.cxx
  igstkImageSpatialObject.cxx
  igstkImageSpatialObjectRepresentation.cxx
  igstkImageReader.cxx
  igstkNDICommandInterpreter.cxx
  igstkMouseTracker.cxx
  igstkTubeObject.cxx
  igstkTubeObjectRepresentation.cxx
  igstkMeshObject.cxx
  igstkMeshObjectRepresentation.cxx
)

# Variable for holding extra libraries
SET(EXTRA_LIBS)

# executed if Atamai classes in Utilities are used
IF( IGSTK_USE_ATAMAI )
  SET(IGSTKSandbox_HEADS ${IGSTKSandbox_HEADS}
    igstkAtamaiNDITracker.h
  )
  SET(IGSTKSandbox_SRCS ${IGSTKSandbox_SRCS}
    igstkAtamaiNDITracker.cxx
  )
  INCLUDE_DIRECTORIES(
    ${ATAMAITRACKING_SOURCE_DIR}
    ${NDICAPI_SOURCE_DIR}
  )
  SET(EXTRA_LIBS ${EXTRA_LIBS} vtkAtamaiTracking)
ENDIF( IGSTK_USE_ATAMAI )

# Adding the IGSTKSandbox library
ADD_LIBRARY(IGSTKSandbox ${IGSTKSandbox_HEADS} ${IGSTKSandbox_SRCS})

# Linking with the IGSTK libraries should bring up all the other dependent
# libraries such as ITK, VTK and FLTK
TARGET_LINK_LIBRARIES( IGSTKSandbox IGSTK ${EXTRA_LIBS} )
