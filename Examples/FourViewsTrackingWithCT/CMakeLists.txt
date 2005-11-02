PROJECT(FourViewsTrackingWithCT)
  
SET(FourViewsTrackingWithCT_GUI_SRCS
  FourViewsTrackingWithCTGUI.fl
  )
  
SET(FourViewsTrackingWithCT_SRCS
  main.cxx
  FourViewsTrackingWithCT.cxx
  )

INCLUDE_DIRECTORIES(
  ${FourViewsTrackingWithCT_SOURCE_DIR}
  ${FourViewsTrackingWithCT_BINARY_DIR}
  )

FLTK_WRAP_UI(FourViewsTrackingWithCT ${FourViewsTrackingWithCT_GUI_SRCS})
ADD_EXECUTABLE(FourViewsTrackingWithCT ${FourViewsTrackingWithCT_FLTK_UI_SRCS} ${FourViewsTrackingWithCT_SRCS})
TARGET_LINK_LIBRARIES(FourViewsTrackingWithCT IGSTKSandbox)


