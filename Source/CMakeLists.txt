#
#  Define the list of source files forming the IGSTKSandbox Library
#
# The header files

SET(IGSTKSandbox_HEADS
  igstkRenderWindow.h
)

# The implementation files

SET(IGSTKSandbox_SRCS  
  igstkRenderWindow.cxx
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

