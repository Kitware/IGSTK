PROJECT(TwoViews)
  
SET(TwoViews_GUI_SRCS
    TwoViews.fl
    )
  
SET(TwoViews_SRCS
    main.cxx
   TrackingBox.h
   )

INCLUDE_DIRECTORIES(
${TwoViews_SOURCE_DIR}
)

LINK_LIBRARIES(
IGSTKSandbox
)

FLTK_WRAP_UI(TwoViews ${TwoViews_GUI_SRCS})
  
ADD_EXECUTABLE(TwoViews ${TwoViews_SRCS})

