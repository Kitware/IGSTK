PROJECT(TwoViews)
  
SET(TwoViews_GUI_SRCS
    TwoViews.fl
    )
  
SET(TwoViews_SRCS
    main.cxx
    )

INCLUDE_DIRECTORIES(
  ${TwoViews_SOURCE_DIR}
  )


FLTK_WRAP_UI(TwoViewsLib ${TwoViews_GUI_SRCS})
ADD_LIBRARY(TwoViewsLib ${TwoViewsLib_FLTK_UI_SRCS})

ADD_EXECUTABLE(TwoViews ${TwoViews_SRCS})

TARGET_LINK_LIBRARIES(TwoViews TwoViewsLib IGSTKSandbox)

