PROJECT(FourViewsAndTracking)
  
SET(FourViewsAndTracking_GUI_SRCS
  FourViewsAndTrackingGUI.fl
  )
  
SET(FourViewsAndTracking_SRCS
  FourViewsAndTracking.cxx
  )


INCLUDE_DIRECTORIES(
  ${FourViewsAndTracking_SOURCE_DIR}
  ${FourViewsAndTracking_BINARY_DIR}
  )

FLTK_WRAP_UI(FourViewsAndTracking ${FourViewsAndTracking_GUI_SRCS})
ADD_EXECUTABLE(FourViewsAndTracking ${FourViewsAndTracking_FLTK_UI_SRCS} ${FourViewsAndTracking_SRCS})
TARGET_LINK_LIBRARIES(FourViewsAndTracking IGSTK)



