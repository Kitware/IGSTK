PROJECT(FourViewsTrackingWithCT)
  
SET(FourViewsTrackingWithCT_GUI_SRCS
  FourViewsTrackingWithCT.fl
  )
  
SET(FourViewsTrackingWithCT_SRCS
  main.cxx
  )

SET(OneViewAndTracking_GUI_SRCS
  OneViewAndTracking.fl
  )
  

INCLUDE_DIRECTORIES(
  ${FourViewsTrackingWithCT_SOURCE_DIR}
  ${FourViewsTrackingWithCT_BINARY_DIR}
  )

LINK_LIBRARIES(
  IGSTKSandbox
  )

FLTK_WRAP_UI(FourViewsTrackingWithCT ${FourViewsTrackingWithCT_GUI_SRCS})

IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.2)
  # For CMake version below 2.2
  ADD_EXECUTABLE(FourViewsTrackingWithCT ${FourViewsTrackingWithCT_SRCS})
ELSE ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.2)
  # For CMake version higher or equal than 2.2
  ADD_EXECUTABLE(FourViewsTrackingWithCT ${FourViewsTrackingWithCT_SRCS} ${FourViewsTrackingWithCT_FLTK_UI_SRCS})
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.2)



