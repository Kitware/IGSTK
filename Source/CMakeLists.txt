#
#  Define the list of source files forming the IGSTK Library
#
# The header files

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

