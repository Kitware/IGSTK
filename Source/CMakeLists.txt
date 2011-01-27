# make a directory to copy latest versions of all source and header files
SET(IGSTK_LATEST_FILES  ${IGSTKSandbox_BINARY_DIR}/SourceLatest )
MAKE_DIRECTORY( ${IGSTK_LATEST_FILES} )

INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_BINARY_DIR}/SourceLatest
)

SET(IGSTK_HEADS
  igstkUSImageObject.h
  igstkUSImageObjectRepresentation.h
  igstkUSImageReader.h
  igstkMR3DImageToUS3DImageRegistration.h
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
  igstkTransformBase.h
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
  igstkView.h
  igstkView2D.h
  igstkView3D.h
  igstkViewProxy.h
  igstkViewProxyBase.h
  igstkLogger.h
  igstkCoordinateSystem.h
  igstkCoordinateSystemDelegator.h
  igstkCoordinateSystemTransformToResult.h
  igstkCoordinateSystemTransformToErrorResult.h
  igstkCoordinateSystemInterfaceMacros.h
  igstkCoordinateSystemSetTransformResult.h

  igstkCircularSimulatedTracker.h
  igstkSimulatedTrackerTool.h
  igstkSimulatedTracker.h
  igstkNDITracker.h

  igstkAffineTransform.h
  igstkPerspectiveTransform.h

  igstkTransformObserver.h


# Ascension tracker support
  igstkAscensionCommandInterpreter.h
  igstkAscensionTracker.h
  igstkAscensionTrackerTool.h

# Reslicing component support
  igstkReslicerPlaneSpatialObject.h

  igstkToolProjectionSpatialObject.h
  igstkToolProjectionObjectRepresentation.h
  igstkMeshResliceObjectRepresentation.h
  igstkImageResliceObjectRepresentation.h
  igstkCrossHairSpatialObject.h
  igstkCrossHairObjectRepresentation.h

  igstkPivotCalibrationAlgorithm.h

  igstkPivotCalibration.h
)

IF(IGSTKSandbox_USE_FLTK)
    SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkFLTKTextBufferLogOutput.h
      igstkFLTKTextLogOutput.h
      igstkMouseTracker.h
      igstkFLTKWidget.h
      igstkFLTKWidget.h
      igstkMouseTrackerTool.h
      igstkPivotCalibrationFLTKWidget.h
      )
ENDIF(IGSTKSandbox_USE_FLTK)

IF(IGSTKSandbox_USE_Qt)
  SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkQMouseTracker.h
      igstkQMouseTrackerTool.h
      igstkQTWidget.h
   )
ENDIF(IGSTKSandbox_USE_Qt)

IF(IGSTKSandbox_USE_MicronTracker)
  SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkMicronTracker.h
      igstkMicronTrackerTool.h
     )
ENDIF(IGSTKSandbox_USE_MicronTracker)

IF(IGSTKSandbox_USE_OpenIGTLink)
    SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkTrackerToolObserverToOpenIGTLinkRelay.h
      )
ENDIF(IGSTKSandbox_USE_OpenIGTLink)

# The implementation files

SET(IGSTK_SRCS
  igstkUSImageObject.cxx
  igstkUSImageObjectRepresentation.cxx
  igstkUSImageReader.cxx
  igstkMR3DImageToUS3DImageRegistration.cxx
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
  igstkLandmark3DRegistration2.cxx
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
  igstkTransformBase.cxx
  igstkTubeObject.cxx
  igstkTubeObjectRepresentation.cxx
  igstkUltrasoundProbeObject.cxx
  igstkUltrasoundProbeObjectRepresentation.cxx
  igstkUltrasoundImageSimulator.txx
  igstkVTKLoggerOutput.cxx
  igstkTubeReader.cxx
  igstkMeshReader.cxx
  igstkGroupObject.cxx
  igstkView.cxx
  igstkView2D.cxx
  igstkView3D.cxx
  igstkViewProxyBase.cxx
  igstkLogger.cxx
  igstkCoordinateSystem.cxx
  igstkCoordinateSystemDelegator.cxx
  igstkCoordinateSystemTransformToResult.cxx
  igstkCoordinateSystemTransformToErrorResult.cxx
  igstkCoordinateSystemSetTransformResult.cxx

  igstkCircularSimulatedTracker.cxx
  igstkSimulatedTrackerTool.cxx
  igstkSimulatedTracker.cxx
  igstkNDITracker.cxx

  igstkAffineTransform.cxx
  igstkPerspectiveTransform.cxx

# Ascension tracker support
  igstkAscensionCommandInterpreter.cxx
  igstkAscensionTracker.cxx
  igstkAscensionTrackerTool.cxx

# Reslicing component support
  igstkReslicerPlaneSpatialObject.cxx

  igstkToolProjectionSpatialObject.cxx
  igstkToolProjectionObjectRepresentation.cxx
  igstkMeshResliceObjectRepresentation.cxx
  igstkImageResliceObjectRepresentation.txx
  igstkCrossHairSpatialObject.cxx
  igstkCrossHairObjectRepresentation.cxx

  igstkPivotCalibrationAlgorithm.cxx

  igstkPivotCalibration.cxx
)

IF(IGSTKSandbox_USE_FLTK)
  SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkMouseTracker.cxx
      igstkFLTKTextBufferLogOutput.cxx
      igstkFLTKTextLogOutput.cxx
      igstkFLTKWidget.cxx
      igstkMouseTrackerTool.cxx
      igstkPivotCalibrationFLTKWidget.cxx
      )
ENDIF(IGSTKSandbox_USE_FLTK)

IF(IGSTKSandbox_USE_Qt)
  SET(IGSTK_SRCS
    ${IGSTK_SRCS}
    igstkQMouseTracker.cxx
    igstkQMouseTrackerTool.cxx
    igstkQTWidget.cxx
    )
ENDIF(IGSTKSandbox_USE_Qt)

IF(IGSTKSandbox_USE_MicronTracker)
  SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkMicronTracker.cxx
      igstkMicronTrackerTool.cxx

     )
ENDIF(IGSTKSandbox_USE_MicronTracker)

IF(IGSTKSandbox_USE_OpenIGTLink)
    SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkTrackerToolObserverToOpenIGTLinkRelay.cxx
      )
ENDIF(IGSTKSandbox_USE_OpenIGTLink)


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

# Video grabber support
  igstkVideoFrameSpatialObject.h
  igstkVideoFrameRepresentation.h
  igstkFrame.h
  igstkVideoImager.h
  igstkVideoImagerTool.h

  igstkPrincipalAxisCalibration.h
  igstkContourMeshObjectRepresentation.h
  igstkContourObjectRepresentation.h
  igstkContourVascularNetworkObjectRepresentation.h
  igstkDICOMGenericImageReader.h
  igstkGenericImageSpatialObject.h
  igstkLandmarkUltrasoundCalibration.h
#  igstkObliqueContourMeshObjectRepresentation.h
#  igstkObliqueImageSpatialObjectRepresentation.h
  igstkPoint3D.h
#  igstkQuickTimeGrabber.h
  igstkSocketCommunication.h
#  igstkVideoGrabber.h
#  igstkImageSliceRepresentation.h

  igstkTubeGroupObject.h

  igstkVascularNetworkObject.h
  igstkVascularNetworkObjectRepresentation.h
  igstkVascularNetworkReader.h
  igstkVesselObject.h
  igstkVesselObjectRepresentation.h

  igstkMeshClippingObjectRepresentation.h

  igstkPETImageReader.h
  igstkPETImageSpatialObject.h
  igstkPETImageSpatialObjectRepresentation.h
)

IF(IGSTKSandbox_USE_FLTK)
ENDIF(IGSTKSandbox_USE_FLTK)      

IF (IGSTKSandbox_USE_OpenIGTLink)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkOpenIGTLinkVideoImager.h
      igstkOpenIGTLinkVideoImager.cxx
      igstkOpenIGTLinkVideoImagerTool.h
      igstkOpenIGTLinkVideoImagerTool.cxx
      )
ENDIF (IGSTKSandbox_USE_OpenIGTLink)

IF (IGSTKSandbox_USE_unicap)
  INCLUDE_DIRECTORIES(
  /usr/local/include/unicap
  )
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkImagingSourceVideoImager.h
      igstkImagingSourceVideoImager.cxx
      igstkImagingSourceVideoImagerTool.h
      igstkImagingSourceVideoImagerTool.cxx
      )
ENDIF (IGSTKSandbox_USE_unicap)

IF (IGSTKSandbox_USE_CompressedDV)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkCompressedDVVideoImager.h
      igstkCompressedDVVideoImager.cxx
      igstkCompressedDVVideoImagerTool.h
      igstkCompressedDVVideoImagerTool.cxx
      )
ENDIF (IGSTKSandbox_USE_CompressedDV)

IF( WIN32 )
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkWebCameraTracker.h
      igstkWebCameraTrackerTool.h
     )
    
      IF(IGSTKSandbox_USE_OpenCV)
         SET(IGSTKSandbox_SRCS
             ${IGSTKSandbox_SRCS}
             igstkWebcamWinVideoImager.h
             igstkWebcamWinVideoImagerTool.h
             )
      ENDIF(IGSTKSandbox_USE_OpenCV)
ENDIF( WIN32 )

IF(IGSTKSandbox_USE_vtkKWImage)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkGenericImageReader.h
#     igstkGenericImageSpatialObjectNew.h
     )
ENDIF(IGSTKSandbox_USE_vtkKWImage)

IF(IGSTKSandbox_USE_MicronTracker)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkMicronVideoImager.h
      igstkMicronVideoImagerTool.h
     )
ENDIF(IGSTKSandbox_USE_MicronTracker)

IF(IGSTKSandbox_USE_NDICertus)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkNDICertusTracker.h
      igstkNDICertusTrackerTool.h
     )
ENDIF(IGSTKSandbox_USE_NDICertus)

IF(IGSTKSandbox_USE_Ascension3DGTracker)
  SET(IGSTKSandbox_HEADS
      ${IGSTKSandbox_HEADS}
      igstkAscension3DGTracker.h
      igstkAscension3DGTrackerTool.h
     )
ENDIF(IGSTKSandbox_USE_Ascension3DGTracker)

# The implementation files
SET(IGSTKSandbox_SRCS


# Video grabber support
  igstkVideoFrameSpatialObject.txx
  igstkVideoFrameRepresentation.txx
  igstkFrame.cxx
  igstkVideoImager.cxx
  igstkVideoImagerTool.cxx

  igstkPrincipalAxisCalibration.cxx

#  igstkContourMeshObjectRepresentation.cxx
  igstkContourObjectRepresentation.cxx
  igstkContourVascularNetworkObjectRepresentation.cxx
  igstkDICOMGenericImageReader.cxx
  igstkGenericImageSpatialObject.cxx
  igstkLandmarkUltrasoundCalibration.cxx
 # igstkObliqueContourMeshObjectRepresentation.cxx
 # igstkObliqueImageSpatialObjectRepresentation.txx
  igstkPoint3D.cxx
#  igstkQuickTimeGrabber.cxx
  igstkSocketCommunication.cxx
#  igstkVideoGrabber.cxx
#  igstkImageSliceRepresentation.txx

  igstkTubeGroupObject.cxx

  igstkVascularNetworkObject.cxx
  igstkVascularNetworkObjectRepresentation.cxx
  igstkVascularNetworkReader.cxx
  igstkVesselObject.cxx
  igstkVesselObjectRepresentation.cxx
  
  igstkMeshClippingObjectRepresentation.cxx

  igstkPETImageReader.cxx
  igstkPETImageSpatialObject.cxx
  igstkPETImageSpatialObjectRepresentation.cxx

)

IF(IGSTKSandbox_USE_FLTK)
ENDIF(IGSTKSandbox_USE_FLTK)      

IF( WIN32 )
  SET(IGSTKSandbox_SRCS
      ${IGSTKSandbox_SRCS}
      igstkWebCameraTracker.cxx
      igstkWebCameraTrackerTool.cxx
     )
             
      IF(IGSTKSandbox_USE_OpenCV)
        SET(IGSTKSandbox_SRCS
            ${IGSTKSandbox_SRCS}
            igstkWebcamWinVideoImager.cxx
            igstkWebcamWinVideoImagerTool.cxx
            )
      ENDIF(IGSTKSandbox_USE_OpenCV)        
ENDIF( WIN32 )

IF(IGSTKSandbox_USE_vtkKWImage)
  SET(IGSTKSandbox_SRCS
      ${IGSTKSandbox_SRCS}
      igstkGenericImageReader.cxx
#      igstkGenericImageSpatialObjectNew.cxx
     )
ENDIF(IGSTKSandbox_USE_vtkKWImage)

IF(IGSTKSandbox_USE_MicronTracker)
  SET(IGSTKSandbox_SRCS
      ${IGSTKSandbox_SRCS}
      igstkMicronVideoImager.cxx
      igstkMicronVideoImagerTool.cxx
     )
ENDIF(IGSTKSandbox_USE_MicronTracker)

IF(IGSTKSandbox_USE_NDICertus)
  SET(IGSTKSandbox_SRCS
      ${IGSTKSandbox_SRCS}
      igstkNDICertusTracker.cxx
      igstkNDICertusTrackerTool.cxx
     )
ENDIF(IGSTKSandbox_USE_NDICertus)

IF(IGSTKSandbox_USE_Ascension3DGTracker)
  Set(IGSTKSandbox_SRCS
      ${IGSTKSandbox_SRCS}
      igstkAscension3DGTracker.cxx
      igstkAscension3DGTrackerTool.cxx
     )
ENDIF(IGSTKSandbox_USE_Ascension3DGTracker)

# Variable for holding extra libraries
SET(EXTRA_LIBS)

IF(IGSTKSandbox_USE_FLTK)
  SET(EXTRA_LIBS ${EXTRA_LIBS} ${FLTK_LIBRARIES})
ENDIF(IGSTKSandbox_USE_FLTK)

IF(IGSTKSandbox_USE_Qt)
  IF(APPLE)
   SET(EXTRA_LIBS ${EXTRA_LIBS} QVTK  "-framework QtTest")
  ELSE(APPLE)
    SET(EXTRA_LIBS ${EXTRA_LIBS} QVTK  ${QT_QTTEST_LIBRARY})
  ENDIF(APPLE)
ENDIF(IGSTKSandbox_USE_Qt)

IF(IGSTKSandbox_USE_MicronTracker)
  SET(EXTRA_LIBS ${EXTRA_LIBS} igstkMTCu)
ENDIF(IGSTKSandbox_USE_MicronTracker)

IF(IGSTKSandbox_USE_Ascension3DGTracker)
  SET(EXTRA_LIBS ${EXTRA_LIBS} ATC3DG)
ENDIF(IGSTKSandbox_USE_Ascension3DGTracker)

IF(IGSTKSandbox_USE_NDICertus)
  SET(EXTRA_LIBS ${EXTRA_LIBS} igstkNDICertus)
ENDIF(IGSTKSandbox_USE_NDICertus)

IF(IGSTKSandbox_USE_OpenIGTLink)
  SET(EXTRA_LIBS ${EXTRA_LIBS} OpenIGTLink)
ENDIF(IGSTKSandbox_USE_OpenIGTLink)

IF(IGSTK_USE_SceneGraphVisualization)
  SET(EXTRA_LIBS ${EXTRA_LIBS} SceneGraphVisualization)
ENDIF(IGSTK_USE_SceneGraphVisualization)

IF (IGSTKSandbox_USE_unicap)
 SET(EXTRA_LIBS ${EXTRA_LIBS} unicap)
ENDIF (IGSTKSandbox_USE_unicap)

IF (IGSTKSandbox_USE_CompressedDV)
 SET(EXTRA_LIBS ${EXTRA_LIBS} raw1394 dv iec61883)
ENDIF (IGSTKSandbox_USE_CompressedDV)

IF(IGSTKSandbox_USE_OpenCV)
SET(EXTRA_LIBS ${EXTRA_LIBS} ${OpenCV_LIBS})
ENDIF(IGSTKSandbox_USE_OpenCV)

IF(IGSTKSandbox_USE_vtkKWImage)
  SET(EXTRA_LIBS ${EXTRA_LIBS} igstkKWImageu)
ENDIF(IGSTKSandbox_USE_vtkKWImage)

# Adding the IGSTKSandbox library
ADD_LIBRARY(IGSTKSandbox ${IGSTKSandbox_HEADS} ${IGSTKSandbox_SRCS} ${IGSTKLatest_HEADS} ${IGSTKLatest_SRCS})

# Linking with the IGSTK libraries should bring up all the other dependent

TARGET_LINK_LIBRARIES(IGSTKSandbox
    ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject ITKStatistics
    vtkVolumeRendering vtkRendering vtkGraphics vtkHybrid vtkImaging
    vtkIO vtkFiltering vtkCommon vtkWidgets ${EXTRA_LIBS})
