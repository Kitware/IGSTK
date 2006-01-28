PROJECT(OneViewAndTracking)
  
SET(OneViewAndTracking_GUI_SRCS
  OneViewAndTrackingGUI.fl
  )
  
SET(OneViewAndTracking_SRCS
  OneViewAndTracking.cxx
  )

FLTK_WRAP_UI(OneViewAndTracking ${OneViewAndTracking_GUI_SRCS})
ADD_EXECUTABLE(OneViewAndTracking ${OneViewAndTracking_FLTK_UI_SRCS} ${OneViewAndTracking_SRCS})
TARGET_LINK_LIBRARIES(OneViewAndTracking IGSTK)



