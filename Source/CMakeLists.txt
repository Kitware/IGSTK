#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkAuroraTracker.h
  igstkView.h
  igstkView2D.h
  igstkView3D.h
  igstkScene.h
  igstkSerialCommunication.h
  igstkSerialCommunicationCommand.h
  igstkSpatialObject.h
  igstkEllipsoidObject.h  
  igstkCylinderObject.h
  igstkObjectRepresentation.h
  igstkEllipsoidObjectRepresentation.h  
  igstkCylinderObjectRepresentation.h
  igstkMouseTracker.h
  igstkTimeStamp.h
  igstkTransform.h
)


# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkAuroraTracker.cxx
  igstkView.cxx
  igstkView2D.cxx
  igstkView3D.cxx
  igstkScene.cxx
  igstkSerialCommunication.cxx
  igstkSpatialObject.cxx
  igstkEllipsoidObject.cxx
  igstkCylinderObject.cxx
  igstkObjectRepresentation.cxx
  igstkEllipsoidObjectRepresentation.cxx
  igstkCylinderObjectRepresentation.cxx
  igstkMouseTracker.cxx
  igstkTimeStamp.cxx
  igstkTransform.cxx
)

IF( WIN32 )
  IF(NOT CYGWIN)
    SET(IGSTKSandbox_HEADS ${IGSTKSandbox_HEADS} 
        igstkSerialCommunicationForWindows.h
        )

    SET(IGSTKSandbox_SRCS ${IGSTKSandbox_SRCS} 
        igstkSerialCommunicationForWindows.cxx
        )
  ENDIF(NOT CYGWIN)
ELSE( WIN32 )
    SET(IGSTKSandbox_HEADS ${IGSTKSandbox_HEADS} 
        igstkSerialCommunicationForLinux.h
        )

    SET(IGSTKSandbox_SRCS ${IGSTKSandbox_SRCS} 
        igstkSerialCommunicationForLinux.cxx
        )
ENDIF( WIN32 )

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

