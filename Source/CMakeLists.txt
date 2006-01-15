#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkMouseTracker.h
  igstkPivotCalibration.h
  igstkPrincipalAxisCalibration.h
  igstkFlockOfBirdsTracker.h
  igstkFlockOfBirdsTrackerTool.h
  igstkUltrasoundProbeObject.h
  igstkUltrasoundProbeObjectRepresentation.h
  igstkAxesObject.h
  igstkAxesObjectRepresentation.h
  igstkAnnotation2D.h
  igstkLandmark3DRegistrationErrorEstimator.h
  igstkBoxObject.h
  igstkBoxObjectRepresentation.h
  igstkConeObject.h
  igstkConeObjectRepresentation.h
)


# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkMouseTracker.cxx
  igstkPivotCalibration.cxx
  igstkPrincipalAxisCalibration.cxx
  igstkFlockOfBirdsTracker.cxx
  igstkFlockOfBirdsTrackerTool.cxx
  igstkUltrasoundProbeObject.cxx
  igstkUltrasoundProbeObjectRepresentation.cxx
  igstkAxesObject.cxx
  igstkAxesObjectRepresentation.cxx
  igstkAnnotation2D.cxx
  igstkLandmark3DRegistrationErrorEstimator.cxx
  igstkBoxObject.cxx
  igstkBoxObjectRepresentation.cxx
  igstkConeObject.cxx
  igstkConeObjectRepresentation.cxx
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
