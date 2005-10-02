PROJECT(FourViews)
  
SET(FourViews_GUI_SRCS
  FourViews.fl
  )
  
SET(FourViews_SRCS
  main.cxx
  )

INCLUDE_DIRECTORIES(
  ${FourViews_SOURCE_DIR}
  ${FourViews_BINARY_DIR}
  )

FLTK_WRAP_UI(FourViews ${FourViews_GUI_SRCS})

ADD_EXECUTABLE(FourViews ${FourViews_FLTK_UI_SRCS} ${FourViews_SRCS})
TARGET_LINK_LIBRARIES(FourViews IGSTKSandbox)



