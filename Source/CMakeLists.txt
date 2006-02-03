# make a directory to copy latest versions of all source and header files
SET(IGSTK_LATEST_FILES  ${IGSTKSandbox_BINARY_DIR}/SourceLatest )
MAKE_DIRECTORY( ${IGSTK_LATEST_FILES} )

INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/SourceLatest
)

SET(IGSTK_HEADS
  igstkAnnotation2D.h
  igstkAxesObject.h
  igstkAxesObjectRepresentation.h
  igstkBoxObject.h
  igstkBoxObjectRepresentation.h
  igstkConeObject.h
  igstkConeObjectRepresentation.h
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
  igstkLandmark3DRegistration.h
  igstkLandmark3DRegistrationErrorEstimator.h
  igstkMouseTracker.h
  igstkAuroraTracker.h
  igstkAuroraTrackerTool.h
  igstkBinaryData.h
  igstkCommunication.h
  igstkCylinderObject.h
  igstkCylinderObjectRepresentation.h
  igstkEllipsoidObject.h  
  igstkEllipsoidObjectRepresentation.h  
  igstkEvents.h
  igstkMacros.h
  igstkMeshObject.h
  igstkMeshObjectRepresentation.h
  igstkMultipleOutput.h
  igstkNDICommandInterpreter.h
  igstkNDIErrorEvent.h
  igstkObjectRepresentation.h
  igstkObject.h
  igstkPivotCalibration.h
  igstkPrincipalAxisCalibration.h
  igstkPolarisTracker.h
  igstkPolarisTrackerTool.h
  igstkPulseGenerator.h
  igstkSerialCommunication.h
  igstkSerialCommunicationSimulator.h
  igstkSpatialObject.h
  igstkStateMachine.h
  igstkStateMachineInput.h
  igstkStateMachineState.h
  igstkTimeStamp.h
  igstkTransform.h
  igstkToken.h
  igstkTracker.h
  igstkTrackerPort.h
  igstkTrackerTool.h
  igstkTubeObject.h
  igstkTubeObjectRepresentation.h
  igstkUltrasoundProbeObject.h
  igstkUltrasoundProbeObjectRepresentation.h
  igstkVTKLoggerOutput.h
  igstkSpatialObjectReader.h
  igstkTubeReader.h
  igstkMeshReader.h
  igstkGroupObject.h
  igstkTubeGroupObject.h
)

IF(IGSTK_USE_FLTK)
  SET(IGSTK_HEADS
    ${IGSTK_HEADS}
    igstkFLTKTextBufferLogOutput.h
    igstkFLTKTextLogOutput.h
    igstkView.h
    igstkView2D.h
    igstkView3D.h
    )
ENDIF(IGSTK_USE_FLTK)

# The implementation files

SET(IGSTK_SRCS  
  igstkAnnotation2D.cxx
  igstkAxesObject.cxx
  igstkAxesObjectRepresentation.cxx
  igstkBoxObject.cxx
  igstkBoxObjectRepresentation.cxx
  igstkConeObject.cxx
  igstkConeObjectRepresentation.cxx
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
  igstkLandmark3DRegistration.cxx
  igstkLandmark3DRegistrationErrorEstimator.cxx
  igstkMouseTracker.cxx
  igstkAuroraTracker.cxx
  igstkAuroraTrackerTool.cxx
  igstkBinaryData.cxx
  igstkCommunication.cxx
  igstkCylinderObject.cxx
  igstkCylinderObjectRepresentation.cxx
  igstkEllipsoidObject.cxx
  igstkEllipsoidObjectRepresentation.cxx
  igstkMeshObject.cxx
  igstkMeshObjectRepresentation.cxx
  igstkMultipleOutput.cxx
  igstkNDICommandInterpreter.cxx
  igstkObject.cxx
  igstkPivotCalibration.cxx
  igstkPrincipalAxisCalibration.cxx
  igstkObjectRepresentation.cxx
  igstkPolarisTracker.cxx
  igstkPolarisTrackerTool.cxx
  igstkPulseGenerator.cxx
  igstkSerialCommunication.cxx
  igstkSerialCommunicationSimulator.cxx
  igstkSpatialObject.cxx
  igstkSpatialObjectReader.txx
  igstkStateMachine.txx
  igstkTimeStamp.cxx
  igstkToken.cxx
  igstkTracker.cxx
  igstkTrackerPort.cxx
  igstkTrackerTool.cxx
  igstkTransform.cxx
  igstkTubeObject.cxx
  igstkTubeObjectRepresentation.cxx
  igstkUltrasoundProbeObject.cxx
  igstkUltrasoundProbeObjectRepresentation.cxx
  igstkVTKLoggerOutput.cxx
  igstkTubeReader.cxx
  igstkMeshReader.cxx
  igstkGroupObject.cxx
  igstkTubeGroupObject.cxx
)

IF(IGSTK_USE_FLTK)
  SET(IGSTK_SRCS  
    ${IGSTK_SRCS}
    igstkFLTKTextBufferLogOutput.cxx
    igstkFLTKTextLogOutput.cxx
    igstkView.cxx
    igstkView2D.cxx
    igstkView3D.cxx
    )
ENDIF(IGSTK_USE_FLTK)

IF( WIN32 )
  SET(IGSTK_HEADS ${IGSTK_HEADS} 
    igstkSerialCommunicationForWindows.h
  )
  SET(IGSTK_SRCS ${IGSTK_SRCS} 
    igstkSerialCommunicationForWindows.cxx
  )
ELSE( WIN32 )
  SET(IGSTK_HEADS ${IGSTK_HEADS} 
    igstkSerialCommunicationForPosix.h
  )
  SET(IGSTK_SRCS ${IGSTK_SRCS} 
    igstkSerialCommunicationForPosix.cxx
  )
ENDIF( WIN32 )

# Copy igstkConfigure.h into latest
CONFIGURE_FILE( ${IGSTK_BINARY_DIR}/igstkConfigure.h ${IGSTK_LATEST_FILES}/igstkConfigure.h COPYONLY)

# Copy the header files if they are not a duplicated in the Sandbox
FOREACH(HeaderFile ${IGSTK_HEADS})

  IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${HeaderFile})
     SET(IGSTKLatest_HEADS ${IGSTKLatest_HEADS} ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${HeaderFile})
     EXEC_PROGRAM("@CMAKE_COMMAND@" ARGS "-E remove \"${IGSTK_LATEST_FILES}/${HeaderFile}\""
                   OUTPUT_VARIABLE rm_out
                   RETURN_VARIABLE rm_retval )
  ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${HeaderFile})
    # Copy the files from IGSTK
    CONFIGURE_FILE( ${IGSTK_SOURCE_DIR}/Source/${HeaderFile} ${IGSTK_LATEST_FILES}/${HeaderFile} COPYONLY)
    SET(IGSTKLatest_HEADS ${IGSTKLatest_HEADS} ${IGSTK_LATEST_FILES}/${HeaderFile})
  ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${HeaderFile})

ENDFOREACH( HeaderFile )

# Copy the source files if they are not a duplicated in the Sandbox
FOREACH(SourceFile ${IGSTK_SRCS})

  IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${SourceFile})
     SET(IGSTKLatest_SRCS ${IGSTKLatest_SRCS} ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${SourceFile})
     EXEC_PROGRAM("@CMAKE_COMMAND@" ARGS "-E remove \"${IGSTK_LATEST_FILES}/${SourceFile}\""
                   OUTPUT_VARIABLE rm_out
                   RETURN_VARIABLE rm_retval )
  ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${SourceFile})
    # Copy the files from IGSTK
    CONFIGURE_FILE( ${IGSTK_SOURCE_DIR}/Source/${SourceFile} ${IGSTK_LATEST_FILES}/${SourceFile} COPYONLY)
    SET(IGSTKLatest_SRCS ${IGSTKLatest_SRCS} ${IGSTK_LATEST_FILES}/${SourceFile})
  ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${SourceFile})

ENDFOREACH( SourceFile )

#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkFlockOfBirdsTracker.h
  igstkFlockOfBirdsTrackerTool.h
)


# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkFlockOfBirdsTracker.cxx
  igstkFlockOfBirdsTrackerTool.cxx
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
ADD_LIBRARY(IGSTKSandbox ${IGSTKSandbox_HEADS} ${IGSTKSandbox_SRCS} ${IGSTKLatest_HEADS} ${IGSTKLatest_SRCS})

# Linking with the IGSTK libraries should bring up all the other dependent
# libraries such as ITK, VTK and FLTK
# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES(IGSTKSandbox
    ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon
    ${FLTK_LIBRARIES} ${EXTRA_LIBS}
)
