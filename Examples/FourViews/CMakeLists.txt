PROJECT(FourViews)
  
SET(FourViews_GUI_SRCS
  FourViews.fl
  )
  
SET(FourViews_SRCS
  main.cxx
  )

INCLUDE_DIRECTORIES(
  ${FourViews_SOURCE_DIR}
  )

FLTK_WRAP_UI(FourViewsLib ${FourViews_GUI_SRCS})
ADD_LIBRARY(FourViewsLib ${FourViewsLib_FLTK_UI_SRCS})

ADD_EXECUTABLE(FourViews ${FourViews_SRCS} ${FourViews_FLTK_UI_SRCS})

TARGET_LINK_LIBRARIES(FourViews FourViewsLib IGSTKSandbox)



