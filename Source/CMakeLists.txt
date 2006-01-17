# make a directory to copy latest versions of all source and header files
SET(IGSTK_LATEST_FILES  ${IGSTKSandbox_BINARY_DIR}/SourceLatest )
MAKE_DIRECTORY( ${IGSTK_LATEST_FILES} )


SET(IGSTK_HEADS
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
  igstkVTKLoggerOutput.h
  igstkSpatialObjectReader.h
  igstkTubeReader.h
  igstkMeshReader.h
  igstkGroupObject.h
  igstkTubeGroupObject.h
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



SET(IGSTKSandbox_HEADS)

FOREACH(HeaderFile ${IGSTK_HEADS})
  IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${HeaderFile})
    SET(HeaderFileFullPath ${IGSTKSandbox_SOURCE_DIR}/Source/${HeaderFile})
  ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${HeaderFile})
    SET(HeaderFileFullPath ${IGSTK_SOURCE_DIR}/Source/${HeaderFile})
  ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${HeaderFile})

# Copy the files
  CONFIGURE_FILE( ${HeaderFileFullPath} ${IGSTK_LATEST_FILES}/${HeaderFile} )
  SET(IGSTKSandbox_HEADS ${IGSTKSandbox_HEADS} ${IGSTK_LATEST_FILES}/${HeaderFile})

ENDFOREACH( HeaderFile )

 
SET(IGSTK_SRCS
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
  igstkObjectRepresentation.cxx
  igstkPolarisTracker.cxx
  igstkPolarisTrackerTool.cxx
  igstkPulseGenerator.cxx
  igstkSerialCommunication.cxx
  igstkSerialCommunicationSimulator.cxx
  igstkSpatialObject.cxx
  igstkStateMachine.txx
  igstkTimeStamp.cxx
  igstkToken.cxx
  igstkTracker.cxx
  igstkTrackerPort.cxx
  igstkTrackerTool.cxx
  igstkTransform.cxx
  igstkTubeObject.cxx
  igstkTubeObjectRepresentation.cxx
  igstkVTKLoggerOutput.cxx
  igstkTubeReader.cxx
  igstkMeshReader.cxx
  igstkGroupObject.cxx
  igstkTubeGroupObject.cxx
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

SET(IGSTKSandbox_SRCS)
FOREACH(SourceFile ${IGSTK_SRCS})
  IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${SourceFile})
    SET(SourceFileFullPath ${IGSTKSandbox_SOURCE_DIR}/Source/${SourceFile})
  ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${SourceFile})
    SET(SourceFileFullPath ${IGSTK_SOURCE_DIR}/Source/${SourceFile})
  ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${SourceFile})

  CONFIGURE_FILE( ${SourceFileFullPath} ${IGSTK_LATEST_FILES}/${SourceFile} )
  SET(IGSTKSandbox_SRCS ${IGSTKSandbox_SRCS} ${IGSTK_LATEST_FILES}/${SourceFile})
ENDFOREACH( SourceFile )

IF( WIN32 )
  SET(SourceFile  igstkSerialCommunicationForWindows.cxx)
  SET(HeaderFile  igstkSerialCommunicationForWindows.h)
ELSE( WIN32 )
  SET(SourceFile  igstkSerialCommunicationForPosix.cxx)
  SET(HeaderFile  igstkSerialCommunicationForPosix.h)
ENDIF( WIN32 )

# add the SerialCommunication files
IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${HeaderFile})
    SET(HeaderFileFullPath ${IGSTKSandbox_SOURCE_DIR}/Source/${HeaderFile})
ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${HeaderFile})
    SET(HeaderFileFullPath ${IGSTK_SOURCE_DIR}/Source/${HeaderFile})
ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${HeaderFile})

CONFIGURE_FILE( ${HeaderFileFullPath} ${IGSTK_LATEST_FILES}/${HeaderFile} )
SET(IGSTKSandbox_HEADS ${IGSTKSandbox_HEADS} ${IGSTK_LATEST_FILES}/${HeaderFile})


IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${SourceFile})
   SET(SourceFileFullPath ${IGSTKSandbox_SOURCE_DIR}/Source/${SourceFile})
ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${SourceFile})
   SET(SourceFileFullPath ${IGSTK_SOURCE_DIR}/Source/${SourceFile})
ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Source/${SourceFile})
  
CONFIGURE_FILE( ${SourceFileFullPath} ${IGSTK_LATEST_FILES}/${SourceFile} )
SET(IGSTKSandbox_SRCS ${IGSTKSandbox_SRCS} ${IGSTK_LATEST_FILES}/${SourceFile})

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

# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES( IGSTKSandbox ${EXTRA_LIBS}
    ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon
    ${FLTK_LIBRARIES}
)

