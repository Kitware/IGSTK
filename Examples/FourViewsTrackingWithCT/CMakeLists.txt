PROJECT(FourViewsTrackingWithCT)
  
SET(FourViewsTrackingWithCT_GUI_SRCS
  FourViewsTrackingWithCT.fl
  )
  
SET(FourViewsTrackingWithCT_SRCS
  main.cxx
  )

INCLUDE_DIRECTORIES(
  ${FourViewsTrackingWithCT_SOURCE_DIR}
  )

FLTK_WRAP_UI(FourViewsTrackingWithCTLib ${FourViewsTrackingWithCT_GUI_SRCS})
ADD_LIBRARY(FourViewsTrackingWithCTLib ${FourViewsTrackingWithCTLib_FLTK_UI_SRCS})
ADD_EXECUTABLE(FourViewsTrackingWithCT ${FourViewsTrackingWithCT_SRCS})

TARGET_LINK_LIBRARIES(FourViewsTrackingWithCT 
  FourViewsTrackingWithCTLib 
  IGSTKSandbox
  )


