# make a directory to copy latest versions of all source and header files
SET(IGSTK_LATEST_FILES  ${IGSTKSandbox_BINARY_DIR}/SourceLatest )
MAKE_DIRECTORY( ${IGSTK_LATEST_FILES} )

INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/SourceLatest
)

SET(IGSTK_HEADS
  igstkFlockOfBirdsCommandInterpreter.h
  igstkFlockOfBirdsTracker.h
  igstkFlockOfBirdsTrackerTool.h
  igstkUSImageObject.h
  igstkUSImageObjectRepresentation.h
  igstkUSImageReader.h
  igstkMR3DImageToUS3DImageRegistration.h
  igstkVascularNetworkObject.h
  igstkVascularNetworkObjectRepresentation.h
  igstkVascularNetworkReader.h
  igstkVesselObject.h
  igstkToolCalibrationReader.h
  igstkToolCalibration.h
  igstkPivotCalibrationReader.h
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
  igstkRealTimeClock.h
  igstkRenderWindowInteractor.h
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
  igstkUltrasoundImageSimulator.h
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
    igstkMouseTracker.h
    igstkView.h
    igstkView2D.h
    igstkView3D.h
    )
ENDIF(IGSTK_USE_FLTK)



# The implementation files

SET(IGSTK_SRCS  
  igstkFlockOfBirdsCommandInterpreter.cxx
  igstkFlockOfBirdsTracker.cxx
  igstkFlockOfBirdsTrackerTool.cxx
  igstkUSImageObject.cxx
  igstkUSImageObjectRepresentation.cxx
  igstkUSImageReader.cxx
  igstkMR3DImageToUS3DImageRegistration.cxx
  igstkVascularNetworkObject.cxx
  igstkVascularNetworkObjectRepresentation.cxx
  igstkVascularNetworkReader.cxx
  igstkVesselObject.cxx
  igstkToolCalibration.cxx
  igstkToolCalibrationReader.txx
  igstkPivotCalibrationReader.cxx
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
  igstkRealTimeClock.cxx
  igstkRenderWindowInteractor.cxx
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
  igstkUltrasoundImageSimulator.txx
  igstkVTKLoggerOutput.cxx
  igstkTubeReader.cxx
  igstkMeshReader.cxx
  igstkGroupObject.cxx
  igstkTubeGroupObject.cxx
)

IF(IGSTK_USE_FLTK)
  SET(IGSTK_SRCS  
    ${IGSTK_SRCS}
    igstkMouseTracker.cxx
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
     SET(IGSTKModifiedFiles ${IGSTKModifiedFiles} ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${HeaderFile})
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
     SET(IGSTKModifiedFiles ${IGSTKModifiedFiles} ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${SourceFile})
  ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${SourceFile})
    # Copy the files from IGSTK
    CONFIGURE_FILE( ${IGSTK_SOURCE_DIR}/Source/${SourceFile} ${IGSTK_LATEST_FILES}/${SourceFile} COPYONLY)
    SET(IGSTKLatest_SRCS ${IGSTKLatest_SRCS} ${IGSTK_LATEST_FILES}/${SourceFile})
  ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source/${SourceFile})

ENDFOREACH( SourceFile )

SOURCE_GROUP(IGSTKModified FILES ${IGSTKModifiedFiles})
#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkObliqueImageSpatialObjectRepresentation.h
  igstkLandmarkUltrasoundCalibration.h
  igstkContourObjectRepresentation.h
  igstkContourMeshObjectRepresentation.h
  igstkContourVascularNetworkObjectRepresentation.h
  igstkSocketCommunication.h
  igstkObliqueContourMeshObjectRepresentation.h
  igstkVideoGrabber.h
)

IF(IGSTKSandbox_USE_Qt)
  SET(IGSTKSandbox_HEADS
    ${IGSTKSandbox_HEADS}
    igstkQMouseTracker.h
    igstkSimulatedTracker.h
    igstkQView.h
    igstkQView2D.h
    igstkQView3D.h
    )
ENDIF(IGSTKSandbox_USE_Qt)


# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkObliqueImageSpatialObjectRepresentation.txx
  igstkLandmarkUltrasoundCalibration.cxx
  igstkContourObjectRepresentation.cxx
  igstkContourMeshObjectRepresentation.cxx
  igstkContourVascularNetworkObjectRepresentation.cxx
  igstkSocketCommunication.cxx
  igstkObliqueContourMeshObjectRepresentation.cxx
  igstkVideoGrabber.cxx
)

IF(IGSTKSandbox_USE_Qt)
  SET(IGSTKSandbox_SRCS  
    ${IGSTKSandbox_SRCS}
    igstkQMouseTracker.cxx
    igstkSimulatedTracker.cxx
    igstkQView.cxx
    igstkQView2D.cxx
    igstkQView3D.cxx
    )
ENDIF(IGSTKSandbox_USE_Qt)


# Variable for holding extra libraries
SET(EXTRA_LIBS)

# Adding the IGSTKSandbox library
ADD_LIBRARY(IGSTKSandbox ${IGSTKSandbox_HEADS} ${IGSTKSandbox_SRCS} ${IGSTKLatest_HEADS} ${IGSTKLatest_SRCS})

# Linking with the IGSTK libraries should bring up all the other dependent
# libraries such as ITK, VTK and FLTK
# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES(IGSTKSandbox
    ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject ITKStatistics
    vtkVolumeRendering vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon
    ${FLTK_LIBRARIES} ${EXTRA_LIBS}
)
