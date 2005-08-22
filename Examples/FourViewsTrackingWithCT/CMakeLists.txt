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
ADD_EXECUTABLE(FourViewsTrackingWithCT ${FourViewsTrackingWithCT_SRCS})


