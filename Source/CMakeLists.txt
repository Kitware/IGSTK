#
#  Define the list of source files forming the IGSTK Library
#
# The header files

SET(IGSTK_HEADS
  igstkCommunication.h
  igstkLogger.h
  igstkMacros.h
  igstkMultipleOutput.h
  igstkStateMachine.h
  igstkTracker.h
  igstkTrackerPort.h
  igstkTrackerTool.h
)

# The implementation files

SET(IGSTK_SRCS  
  igstkCommunication.cxx
  igstkLogger.cxx
  igstkStateMachine.txx
  igstkTracker.cxx
  igstkTrackerPort.cxx
  igstkTrackerTool.cxx
)

# Adding the IGSTK library

ADD_LIBRARY(IGSTK ${IGSTK_HEADS} ${IGSTK_SRCS})

# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES( IGSTK
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon vtkPatented
    ${FLTK_LIBRARIES}
)

