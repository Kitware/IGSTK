#
#  Define the list of source files forming the IGSTK Library
#
# The header files

SET(IGSTK_HEADS
  igstkCommunication.h
  igstkCylinderObject.h
  igstkCylinderObjectRepresentation.h
  igstkEllipsoidObject.h  
  igstkEllipsoidObjectRepresentation.h  
  igstkLogger.h
  igstkMacros.h
  igstkMultipleOutput.h
  igstkObjectRepresentation.h
  igstkScene.h
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
  igstkView.h
  igstkView2D.h
  igstkView3D.h
)

# The implementation files

SET(IGSTK_SRCS  
  igstkCommunication.cxx
  igstkCylinderObject.cxx
  igstkCylinderObjectRepresentation.cxx
  igstkEllipsoidObject.cxx
  igstkEllipsoidObjectRepresentation.cxx
  igstkLogger.cxx
  igstkMultipleOutput.cxx
  igstkObjectRepresentation.cxx
  igstkScene.cxx
  igstkSpatialObject.cxx
  igstkStateMachine.txx
  igstkTimeStamp.cxx
  igstkToken.cxx
  igstkTracker.cxx
  igstkTrackerPort.cxx
  igstkTrackerTool.cxx
  igstkTransform.cxx
  igstkView.cxx
  igstkView2D.cxx
  igstkView3D.cxx
)

# Adding the IGSTK library

ADD_LIBRARY(IGSTK ${IGSTK_HEADS} ${IGSTK_SRCS})

# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES( IGSTK
    ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon vtkPatented
    ${FLTK_LIBRARIES}
)

