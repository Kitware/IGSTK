#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkView.h
  igstkView2D.h
  igstkView3D.h
  igstkScene.h
  igstkSpatialObject.h
  igstkEllipsoidObject.h  
  igstkCylinderObject.h
  igstkObjectRepresentation.h
  igstkEllipsoidObjectRepresentation.h  
  igstkCylinderObjectRepresentation.h
  igstkMouseTracker.h
)

# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkView.cxx
  igstkView2D.cxx
  igstkView3D.cxx
  igstkScene.cxx
  igstkSpatialObject.cxx
  igstkEllipsoidObject.cxx
  igstkCylinderObject.cxx
  igstkObjectRepresentation.cxx
  igstkEllipsoidObjectRepresentation.cxx
  igstkCylinderObjectRepresentation.cxx
  igstkMouseTracker.cxx
)

# Adding the IGSTKSandbox library
ADD_LIBRARY(IGSTKSandbox ${IGSTKSandbox_HEADS} ${IGSTKSandbox_SRCS})

# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES( IGSTKSandbox
    IGSTK
    ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon vtkPatented
    ${FLTK_LIBRARIES}
)

