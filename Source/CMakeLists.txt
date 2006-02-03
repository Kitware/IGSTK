#
#  Define the list of source files forming the IGSTK Library
#
# The header files

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
  igstkMouseTracker.h
  igstkNDICommandInterpreter.h
  igstkNDIErrorEvent.h
  igstkObjectRepresentation.h
  igstkObject.h
  igstkPivotCalibration.h
  igstkPolarisTracker.h
  igstkPolarisTrackerTool.h
  igstkPrincipalAxisCalibration.h
  igstkPulseGenerator.h
  igstkRealTimeClock.h
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
  igstkMouseTracker.cxx
  igstkNDICommandInterpreter.cxx
  igstkObject.cxx
  igstkPivotCalibration.cxx
  igstkObjectRepresentation.cxx
  igstkPolarisTracker.cxx
  igstkPolarisTrackerTool.cxx
  igstkPrincipalAxisCalibration.cxx
  igstkPulseGenerator.cxx
  igstkRealTimeClock.cxx
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


# Adding the IGSTK library

ADD_LIBRARY(IGSTK ${IGSTK_HEADS} ${IGSTK_SRCS})

# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES( IGSTK
    ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon
    ${FLTK_LIBRARIES}
)

