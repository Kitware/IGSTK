#
#  Define the list of source files forming the IGSTK Library
#
# The header files

SET(IGSTK_HEADS
  igstkUSImageObject.h
  igstkUSImageObjectRepresentation.h
  igstkUSImageReader.h
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
  igstkRenderWindowInteractor.h
  igstkRealTimeClock.h
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
  igstkVTKLoggerOutput.h
  igstkSpatialObjectReader.h
  igstkTubeReader.h
  igstkMeshReader.h
  igstkGroupObject.h
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

  igstkView.h
  igstkView2D.h
  igstkView3D.h

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

  igstkPETImageReader.h
  igstkPETImageSpatialObject.h
  igstkPETImageSpatialObjectRepresentation.h

  igstkNDICommandInterpreterClassic.h
  igstkPolarisClassicTracker.h
)

IF(IGSTK_USE_FLTK)
  SET(IGSTK_HEADS
    ${IGSTK_HEADS}
    igstkFLTKTextBufferLogOutput.h
    igstkFLTKTextLogOutput.h
    igstkMouseTracker.h
    igstkMouseTrackerTool.h
    igstkFLTKWidget.h
    igstkPivotCalibrationFLTKWidget.h
    )
ENDIF(IGSTK_USE_FLTK)

IF(IGSTK_USE_Qt)
  SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkQMouseTracker.h
      igstkQMouseTrackerTool.h
      igstkQTWidget.h
   )
ENDIF(IGSTK_USE_Qt)

IF(IGSTK_USE_MicronTracker)
  SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkMicronTracker.h
      igstkMicronTrackerTool.h
     )
ENDIF(IGSTK_USE_MicronTracker)

IF( IGSTK_USE_ArucoTracker )
  SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkArucoTracker.h
      igstkArucoTrackerTool.h
      )
ENDIF( IGSTK_USE_ArucoTracker )

IF(IGSTK_USE_InfiniTrack)
  SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkInfiniTrackTracker.h
      igstkInfiniTrackTrackerTool.h
     )
ENDIF(IGSTK_USE_InfiniTrack)

IF(IGSTK_USE_Ascension3DGTracker)
  SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkAscension3DGTracker.h
      igstkAscension3DGTrackerTool.h
     )
ENDIF(IGSTK_USE_Ascension3DGTracker)

IF(IGSTK_USE_AtracsysEasyTrack500)
    SET(IGSTK_HEADS
    ${IGSTK_HEADS}
    igstkAtracsysEasyTrackTracker.h
    igstkAtracsysEasyTrackTrackerTool.h
    )
ENDIF(IGSTK_USE_AtracsysEasyTrack500)

IF(IGSTK_USE_AXIOS3D)
  SET(IGSTK_HEADS
  ${IGSTK_HEADS}
  igstkAxios3DTracker.h
  igstkAxios3DTrackerTool.h
  )
ENDIF(IGSTK_USE_AXIOS3D)

IF(IGSTK_USE_NDICertusTracker)
  SET(IGSTK_HEADS
	  ${IGSTK_HEADS}
	  igstkNDICertusTracker.h
	  igstkNDICertusTrackerTool.h
	  )
ENDIF(IGSTK_USE_NDICertusTracker)

IF(IGSTK_USE_OpenIGTLink)
    SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkTrackerToolObserverToOpenIGTLinkRelay.h
      )
ENDIF(IGSTK_USE_OpenIGTLink)

IF(IGSTK_USE_VideoImager)
    SET(IGSTK_HEADS
        ${IGSTK_HEADS}
        igstkVideoImager.h
        igstkVideoImagerTool.h
        igstkFrame.h
        igstkVideoFrameSpatialObject.h
        igstkVideoFrameRepresentation.h
        )

  IF(IGSTK_USE_OpenIGTLink)
    SET(IGSTK_HEADS
       ${IGSTK_HEADS}
       igstkOpenIGTLinkVideoImager.h
       igstkOpenIGTLinkVideoImagerTool.h
       )
  ENDIF(IGSTK_USE_OpenIGTLink)

  IF(WIN32)
    IF(IGSTK_USE_OpenCV)
      SET(IGSTK_HEADS
        ${IGSTK_HEADS}
        igstkWebcamWinVideoImager.h
        igstkWebcamWinVideoImagerTool.h
        )
    ENDIF(IGSTK_USE_OpenCV)
  ENDIF(WIN32)

ENDIF(IGSTK_USE_VideoImager)

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
  igstkRenderWindowInteractor.cxx
  igstkRealTimeClock.cxx
  igstkSerialCommunication.cxx
  igstkSerialCommunicationSimulator.cxx
  igstkSpatialObject.cxx
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
  igstkVTKLoggerOutput.cxx
  igstkTubeReader.cxx
  igstkMeshReader.cxx
  igstkGroupObject.cxx
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

  igstkView.cxx
  igstkView2D.cxx
  igstkView3D.cxx
  igstkViewProxyBase.cxx

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

  igstkPETImageReader.cxx
  igstkPETImageSpatialObject.cxx
  igstkPETImageSpatialObjectRepresentation.cxx

  igstkNDICommandInterpreterClassic.cxx
  igstkPolarisClassicTracker.cxx
)

IF(IGSTK_USE_FLTK)
  SET(IGSTK_SRCS  
    ${IGSTK_SRCS}
    igstkFLTKTextBufferLogOutput.cxx
    igstkFLTKTextLogOutput.cxx
    igstkMouseTracker.cxx
    igstkMouseTrackerTool.cxx
    igstkFLTKWidget.cxx
    igstkPivotCalibrationFLTKWidget.cxx
    )
ENDIF(IGSTK_USE_FLTK)

IF(IGSTK_USE_Qt)
  SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkQMouseTracker.cxx
      igstkQMouseTrackerTool.cxx
      igstkQTWidget.cxx
   )
ENDIF(IGSTK_USE_Qt)

IF(IGSTK_USE_MicronTracker)
  SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkMicronTracker.cxx
      igstkMicronTrackerTool.cxx
 
     )
ENDIF(IGSTK_USE_MicronTracker)

IF( IGSTK_USE_ArucoTracker )
  SET(IGSTK_HEADS
      ${IGSTK_HEADS}
      igstkArucoTracker.cxx
      igstkArucoTrackerTool.cxx
      )
ENDIF( IGSTK_USE_ArucoTracker )

IF(IGSTK_USE_InfiniTrack)
  SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkInfiniTrackTracker.cxx
      igstkInfiniTrackTrackerTool.cxx
 
     )
ENDIF(IGSTK_USE_InfiniTrack)

IF(IGSTK_USE_Ascension3DGTracker)
  Set(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkAscension3DGTracker.cxx
      igstkAscension3DGTrackerTool.cxx
     )
ENDIF(IGSTK_USE_Ascension3DGTracker)

IF(IGSTK_USE_AtracsysEasyTrack500)
    SET(IGSTK_SRCS
    ${IGSTK_SRCS}
    igstkAtracsysEasyTrackTracker.cxx
    igstkAtracsysEasyTrackTrackerTool.cxx
    )

    SET( EXTRA_LIBS ${EXTRA_LIBS} atracsysTrackingLib usb-1.0 )
ENDIF(IGSTK_USE_AtracsysEasyTrack500)

IF(IGSTK_USE_AXIOS3D)
  SET(IGSTK_SRCS
  ${IGSTK_SRCS}
  igstkAxios3DTracker.cxx
  igstkAxios3DTrackerTool.cxx
  )

  SET(EXTRA_LIBS ${EXTRA_LIBS} Metrology_DLLImport )
ENDIF(IGSTK_USE_AXIOS3D)

IF(IGSTK_USE_NDICertusTracker)
  SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkNDICertusTracker.cxx
      igstkNDICertusTrackerTool.cxx
     )
ENDIF(IGSTK_USE_NDICertusTracker)

IF(IGSTK_USE_OpenIGTLink)
    SET(IGSTK_SRCS
      ${IGSTK_SRCS}
      igstkTrackerToolObserverToOpenIGTLinkRelay.cxx
      )
ENDIF(IGSTK_USE_OpenIGTLink)

IF(IGSTK_USE_VideoImager)
    SET(IGSTK_SRCS
        ${IGSTK_SRCS}
        igstkVideoImager.cxx
        igstkVideoImagerTool.cxx
        igstkFrame.cxx
        igstkVideoFrameSpatialObject.txx
        igstkVideoFrameRepresentation.txx
        )

  IF(IGSTK_USE_OpenIGTLink)
    SET(IGSTK_SRCS
       ${IGSTK_SRCS}
       igstkOpenIGTLinkVideoImager.cxx
       igstkOpenIGTLinkVideoImagerTool.cxx
       )
  ENDIF(IGSTK_USE_OpenIGTLink)

  IF(WIN32)
    IF(IGSTK_USE_OpenCV)
      SET(IGSTK_SRCS
        ${IGSTK_SRCS}
        igstkWebcamWinVideoImager.cxx
        igstkWebcamWinVideoImagerTool.cxx
        )
    ENDIF(IGSTK_USE_OpenCV)    
  ENDIF(WIN32)

ENDIF(IGSTK_USE_VideoImager)


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

# Variable for holding extra libraries
SET(EXTRA_LIBS)
IF(IGSTK_USE_Qt)
  IF(APPLE)
   SET(EXTRA_LIBS ${EXTRA_LIBS} QVTK  "-framework QtTest")
  ELSE(APPLE)
    SET(EXTRA_LIBS ${EXTRA_LIBS} QVTK  ${QT_QTTEST_LIBRARY})
  ENDIF(APPLE)
ENDIF(IGSTK_USE_Qt)

IF(IGSTK_USE_MicronTracker)
  SET(EXTRA_LIBS ${EXTRA_LIBS} igstkMTCu)
ENDIF(IGSTK_USE_MicronTracker)

IF( IGSTK_USE_ArucoTracker )
  SET(EXTRA_LIBS ${EXTRA_LIBS} aruco)
ENDIF( IGSTK_USE_ArucoTracker )


IF(IGSTK_USE_InfiniTrack)
  SET(EXTRA_LIBS ${EXTRA_LIBS}  v9ITK )
ENDIF(IGSTK_USE_InfiniTrack)

IF(IGSTK_USE_OpenIGTLink)
  SET(EXTRA_LIBS ${EXTRA_LIBS} OpenIGTLink)
ENDIF(IGSTK_USE_OpenIGTLink)

IF(IGSTK_USE_SceneGraphVisualization)
  SET(EXTRA_LIBS ${EXTRA_LIBS} SceneGraphVisualization)
ENDIF(IGSTK_USE_SceneGraphVisualization)

IF(IGSTK_USE_NDICertusTracker)
  IF( WIN32 )
    SET(EXTRA_LIBS ${EXTRA_LIBS} oapi.lib)
  ELSE( WIN32 )
    SET(EXTRA_LIBS ${EXTRA_LIBS} NDItb.lib)
  ENDIF( WIN32 )
ENDIF(IGSTK_USE_NDICertusTracker)

IF(IGSTK_USE_OpenCV)
  SET(EXTRA_LIBS ${EXTRA_LIBS} ${OpenCV_LIBS})
ENDIF(IGSTK_USE_OpenCV)

IF(IGSTK_USE_Ascension3DGTracker)
  SET(EXTRA_LIBS ${EXTRA_LIBS} ${ATC_LIBRARY})
ENDIF(IGSTK_USE_Ascension3DGTracker)


# Adding the IGSTK library

ADD_LIBRARY(IGSTK ${IGSTK_HEADS} ${IGSTK_SRCS})

# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES( IGSTK
    ITKCommon
	ITKOptimizers
    ITKIOMeta
    ITKIOXML
    ITKSpatialObjects
    ITKVTK
    ITKIOJPEG
    ITKIOGDCM
    ITKIOTIFF
    ITKIOPNG
    ITKIOLSM
    ITKIOVTK
    ITKIOStimulate
    ITKIOHDF5
    ITKIOBMP
    ITKIOBioRad
    ITKIONIFTI
    ITKIONRRD
    ITKIOGIPL
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon vtksys
    ${EXTRA_LIBS}
)

# Apply user-defined properties to the library target.
IF(IGSTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(IGSTK PROPERTIES
     ${IGSTK_LIBRARY_PROPERTIES}
  )
ENDIF(IGSTK_LIBRARY_PROPERTIES)

IF(IGSTK_USE_FLTK)
  TARGET_LINK_LIBRARIES( IGSTK ${FLTK_LIBRARIES} )
ENDIF(IGSTK_USE_FLTK)

INSTALL(TARGETS IGSTK
   DESTINATION ${IGSTK_INSTALL_LIB_DIR}
)

FILE(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")

INSTALL(FILES ${__files1} ${__files2}
   DESTINATION ${IGSTK_INSTALL_INCLUDE_DIR}
)
