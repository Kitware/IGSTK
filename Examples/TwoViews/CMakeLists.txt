PROJECT(TwoViews)
  
SET(TwoViews_GUI_SRCS
    TwoViews.fl
    )
  
SET(TwoViews_SRCS
    main.cxx
    )

INCLUDE_DIRECTORIES(
  ${TwoViews_SOURCE_DIR}
  ${TwoViews_BINARY_DIR}
  )


FLTK_WRAP_UI(TwoViews ${TwoViews_GUI_SRCS})
ADD_EXECUTABLE(TwoViews ${TwoViews_FLTK_UI_SRCS} ${TwoViews_SRCS})
TARGET_LINK_LIBRARIES(TwoViews IGSTKSandbox)

