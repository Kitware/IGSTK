#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkView.h
  igstkView2D.h
  igstkView3D.h
  igstkScene.h
  igstkObjectRepresentation.h
  igstkEllipsoidObject.h  
  igstkCylinderObject.h
  igstkMouseTracker.h
  itkCylinderSpatialObject.h
)

# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkView.cxx
  igstkView2D.cxx
  igstkView3D.cxx
  igstkScene.cxx
  igstkObjectRepresentation.cxx
  igstkEllipsoidObject.cxx
  igstkCylinderObject.cxx
  igstkMouseTracker.cxx
  itkCylinderSpatialObject.cxx
)

# Adding the IGSTKSandbox library
ADD_LIBRARY(IGSTKSandbox ${IGSTKSandbox_HEADS} ${IGSTKSandbox_SRCS})

# Linking with the FLTK, ITK and VTK libraries
TARGET_LINK_LIBRARIES( IGSTKSandbox
    IGSTK
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon vtkPatented
    ${FLTK_LIBRARIES}
)

