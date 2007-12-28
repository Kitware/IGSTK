# make a directory to copy latest versions of all source and header files
SET(IGSTK_LATEST_FILES  ${IGSTKSandbox_BINARY_DIR}/SourceLatest )
MAKE_DIRECTORY( ${IGSTK_LATEST_FILES} )

INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/SourceLatest
)

SET(IGSTK_HEADS
# FIXME: comment out the FlockOfBirds class just for the
# time being. It requires major work to make it consistent 
# with the new Tracker API
#  igstkFlockOfBirdsCommandInterpreter.h
#  igstkFlockOfBirdsTracker.h
#  igstkFlockOfBirdsTrackerTool.h
  igstkUSImageObject.h
  igstkUSImageObjectRepresentation.h
  igstkUSImageReader.h
  igstkMR3DImageToUS3DImageRegistration.h
  igstkVascularNetworkObject.h
  igstkVascularNetworkObjectRepresentation.h
  igstkVascularNetworkReader.h
  igstkVesselObject.h
  igstkVesselObjectRepresentation.h
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
  igstkView.h
  igstkView2D.h
  igstkView3D.h
)

IF(IGSTK_USE_FLTK)
    SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkFLTKTextBufferLogOutput.h
      igstkFLTKTextLogOutput.h
      igstkMouseTracker.h
      )
ENDIF(IGSTK_USE_FLTK)

# The implementation files

SET(IGSTK_SRCS
# FIXME: comment out the FlockOfBirds class just for the
# time being. It requires major work to make it consistent 
# with the new Tracker API
#
#  igstkFlockOfBirdsCommandInterpreter.cxx
#  igstkFlockOfBirdsTracker.cxx
#  igstkFlockOfBirdsTrackerTool.cxx
  igstkUSImageObject.cxx
  igstkUSImageObjectRepresentation.cxx
  igstkUSImageReader.cxx
  igstkMR3DImageToUS3DImageRegistration.cxx
  igstkVascularNetworkObject.cxx
  igstkVascularNetworkObjectRepresentation.cxx
  igstkVascularNetworkReader.cxx
  igstkVesselObject.cxx
  igstkVesselObjectRepresentation.cxx
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
  igstkView.cxx
  igstkView2D.cxx
  igstkView3D.cxx
)

IF(IGSTK_USE_FLTK)
  SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkMouseTracker.cxx
      igstkFLTKTextBufferLogOutput.cxx
      igstkFLTKTextLogOutput.cxx
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

# Configure igstkConfigure.h into latest
CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/igstkConfigure.h.in
               ${IGSTK_LATEST_FILES}/igstkConfigure.h
               @ONLY IMMEDIATE)


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
  igstkContourMeshObjectRepresentation.h
  igstkContourObjectRepresentation.h
  igstkContourVascularNetworkObjectRepresentation.h
  igstkCoordinateReferenceSystem.h
  igstkCoordinateReferenceSystemDelegator.h
  igstkCrosshairs2D.h
  igstkCircularSimulatedTracker.h
  igstkSimulatedTrackerTool.h
  igstkDICOMGenericImageReader.h
  igstkGenericImageSpatialObject.h
  igstkLandmarkUltrasoundCalibration.h
  igstkObliqueContourMeshObjectRepresentation.h
  igstkObliqueImageSpatialObjectRepresentation.h
  igstkPivotCalibrationNew.h
  igstkPivotCalibrationAlgorithm.h
  igstkPoint3D.h
  igstkSimulatedTracker.h
  igstkSocketCommunication.h
  igstkVideoGrabber.h
  igstkImageSliceRepresentation.h
# vtkImagePlaneWidget2D.h
# vtkImagePlaneWidget2DInteractionListener.h

  igstkLogger.h
)

IF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)
  SET(IGSTKSandbox_HEADS
    ${IGSTKSandbox_HEADS}
    igstkTrackerToolObserverToSocketRelay.h
    )
ENDIF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)

IF(IGSTKSandbox_USE_Qt)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkQMouseTracker.h
      igstkQMouseTrackerTool.h
      igstkQTWidget.h
#      igstkImageSpatialObjectImagePlaneRepresentation.h
#      igstkCTImageSpatialObjectImagePlaneRepresentation.h
#      igstkMRImageSpatialObjectImagePlaneRepresentation.h
   )
ENDIF(IGSTKSandbox_USE_Qt)

IF(IGSTKSandbox_USE_FLTK)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkFLTKWidget.h
      igstkPivotCalibrationFLTKWidget.h
      igstkMouseTrackerTool.h
     )
ENDIF(IGSTKSandbox_USE_FLTK)

IF(IGSTKSandbox_USE_MicronTracker)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkMicronTracker.h
      igstkMicronTrackerTool.h
     )
ENDIF(IGSTKSandbox_USE_MicronTracker)

# The implementation files
SET(IGSTKSandbox_SRCS
  igstkContourMeshObjectRepresentation.cxx
  igstkContourObjectRepresentation.cxx
  igstkContourVascularNetworkObjectRepresentation.cxx
  igstkCoordinateReferenceSystem.cxx
  igstkCoordinateReferenceSystemDelegator.cxx
  igstkCrosshairs2D.cxx
  igstkCircularSimulatedTracker.cxx
  igstkSimulatedTrackerTool.cxx
  igstkDICOMGenericImageReader.cxx
  igstkGenericImageSpatialObject.cxx
  igstkLandmarkUltrasoundCalibration.cxx
  igstkObliqueContourMeshObjectRepresentation.cxx
  igstkObliqueImageSpatialObjectRepresentation.txx
  igstkPivotCalibrationNew.cxx
  igstkPivotCalibrationAlgorithm.cxx
  igstkPoint3D.cxx
  igstkSocketCommunication.cxx
  igstkVideoGrabber.cxx
  igstkViewProxyBase.cxx
  igstkImageSliceRepresentation.txx
#  igstkImageSpatialObjectImagePlaneRepresentation.txx
#  igstkCTImageSpatialObjectImagePlaneRepresentation.cxx
#  igstkMRImageSpatialObjectImagePlaneRepresentation.cxx
#  vtkImagePlaneWidget2D.cxx
#  vtkImagePlaneWidget2DInteractionListener.cxx

  igstkLogger.cxx
  igstkSimulatedTracker.cxx
)

IF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)
  SET(IGSTKSandbox_SRCS
    ${IGSTKSandbox_SRCS}
    igstkTrackerToolObserverToSocketRelay.cxx
    )
ENDIF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)

IF(IGSTKSandbox_USE_Qt)
  SET(IGSTKSandbox_SRCS
    ${IGSTKSandbox_SRCS}
    igstkQMouseTracker.cxx
    igstkQMouseTrackerTool.cxx
    igstkQTWidget.cxx
    )
ENDIF(IGSTKSandbox_USE_Qt)

IF(IGSTKSandbox_USE_FLTK)
  SET(IGSTKSandbox_SRCS
    ${IGSTKSandbox_SRCS}
    igstkFLTKWidget.cxx
    igstkMouseTrackerTool.cxx
    igstkPivotCalibrationFLTKWidget.cxx
    )
ENDIF(IGSTKSandbox_USE_FLTK)

IF(IGSTKSandbox_USE_MicronTracker)
  SET(IGSTKSandbox_SRCS
      ${IGSTKSandbox_SRCS}
      igstkMicronTracker.cxx
      igstkMicronTrackerTool.cxx
 
     )
ENDIF(IGSTKSandbox_USE_MicronTracker)

# Variable for holding extra libraries
SET(EXTRA_LIBS)

IF(IGSTKSandbox_USE_FLTK)
  SET(EXTRA_LIBS ${EXTRA_LIBS} ${FLTK_LIBRARIES})
ENDIF(IGSTKSandbox_USE_FLTK)

IF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)
  SET(EXTRA_LIBS ${EXTRA_LIBS} vtkParallel)
ENDIF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)


IF(IGSTKSandbox_USE_Qt)
  IF(APPLE)
   SET(EXTRA_LIBS ${EXTRA_LIBS} QVTK  "-framework QtTest")
  ELSE(APPLE)
    SET(EXTRA_LIBS ${EXTRA_LIBS} QVTK  ${QT_QTTEST_LIBRARY})
  ENDIF(APPLE)
ENDIF(IGSTKSandbox_USE_Qt)

IF(IGSTKSandbox_USE_MicronTracker)
  SET(EXTRA_LIBS ${EXTRA_LIBS} igstkMTCu)
   
  IF(WIN32)
    SET(EXTRA_LIBS ${EXTRA_LIBS}  comctl32 wsock32 odbc32 odbccp32 )
  ELSE(WIN32)
    SET(EXTRA_LIBS ${EXTRA_LIBS} raw1394 dc1394_control )
  ENDIF(WIN32)
  
ENDIF(IGSTKSandbox_USE_MicronTracker)

# Adding the IGSTKSandbox library
ADD_LIBRARY(IGSTKSandbox ${IGSTKSandbox_HEADS} ${IGSTKSandbox_SRCS} ${IGSTKLatest_HEADS} ${IGSTKLatest_SRCS})

# Linking with the IGSTK libraries should bring up all the other dependent

TARGET_LINK_LIBRARIES(IGSTKSandbox
    ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject ITKStatistics
    vtkVolumeRendering vtkRendering vtkGraphics vtkHybrid vtkImaging
    vtkIO vtkFiltering vtkCommon
    ${EXTRA_LIBS} )
