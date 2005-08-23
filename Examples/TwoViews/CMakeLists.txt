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

IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.2)
  # For CMake version below 2.2
  ADD_EXECUTABLE(TwoViews ${TwoViews_SRCS})
ELSE ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.2)
  # For CMake version higher or equal than 2.2
  ADD_EXECUTABLE(TwoViews ${TwoViews_SRCS} ${TwoViews_FLTK_UI_SRCS})
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.2)


