#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkImageReader.h
  igstkDICOMImageReader.h
  igstkCTImageReader.h
  igstkMRImageReader.h
  igstkImageSpatialObject.h
  igstkCTImageSpatialObject.h
  igstkMRImageSpatialObject.h
  igstkImageSpatialObjectRepresentation.h
  igstkCTImageSpatialObjectRepresentation.h
  igstkMRImageSpatialObjectRepresentation.h
  igstkMouseTracker.h
  igstkPolarisTracker.h
  igstkPolarisTrackerTool.h
  igstkLandmarkRegistration.h
  igstkLandmark3DRegistration.h
  igstkStringEvent.h
  igstkSpatialObjectReader.h
  igstkTubeReader.h
  igstkMeshReader.h
  igstkGroupObject.h
  igstkTubeGroupObject.h
)


# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkImageReader.txx
  igstkDICOMImageReader.txx
  igstkCTImageReader.cxx
  igstkMRImageReader.cxx
  igstkImageSpatialObject.txx
  igstkCTImageSpatialObject.cxx
  igstkMRImageSpatialObject.cxx
  igstkImageSpatialObjectRepresentation.txx
  igstkCTImageSpatialObjectRepresentation.cxx
  igstkMRImageSpatialObjectRepresentation.cxx
  igstkMouseTracker.cxx
  igstkPolarisTracker.cxx
  igstkPolarisTrackerTool.cxx
  igstkLandmarkRegistration.txx
  igstkLandmark3DRegistration.cxx
  igstkTubeReader.cxx
  igstkMeshReader.cxx
  igstkGroupObject.cxx
  igstkTubeGroupObject.cxx
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
