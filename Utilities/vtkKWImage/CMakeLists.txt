SET(vtkKWImage_SRC
    vtkKWImage.h
    vtkKWImage.cxx
    vtkKWImageIO.h
    vtkKWImageIO.cxx
)

SET(RequiredLibraries
  ITKCommon
  ITKBasicFilters
  ITKIO
  vtkIO
  vtkImaging
)

ADD_LIBRARY(igstkKWImageu ${vtkKWImage_SRC} )
TARGET_LINK_LIBRARIES(igstkKWImageu ${RequiredLibraries})
