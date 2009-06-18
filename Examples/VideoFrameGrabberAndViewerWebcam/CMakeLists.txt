PROJECT(VideoFrameGrabberAndViewerWebcamWin)
 
SET(VideoFrameGrabberAndViewerWebcamWin_GUI_SRCS
    VideoFrameGrabberAndViewerWebcamWinGUI.fl
  )
  
SET(VideoFrameGrabberAndViewerWebcamWin_SRCS
  main.cxx
  VideoFrameGrabberAndViewerWebcamWin.h
  VideoFrameGrabberAndViewerWebcamWin.cxx
  VideoFrameGrabberAndViewerWebcamWinView.h
  VideoFrameGrabberAndViewerWebcamWinView.cxx
  )
  
INCLUDE_DIRECTORIES(
  ${IGSTK_BINARY_DIR}
  ${VideoFrameGrabberAndViewerWebcamWin_SOURCE_DIR}
  ${VideoFrameGrabberAndViewerWebcamWin_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Examples/VideoFrameGrabberAndViewerWebcamWin
  )

FLTK_WRAP_UI(VideoFrameGrabberAndViewerWebcamWin ${VideoFrameGrabberAndViewerWebcamWin_GUI_SRCS})
ADD_EXECUTABLE(VideoFrameGrabberAndViewerWebcamWin ${VideoFrameGrabberAndViewerWebcamWin_FLTK_UI_SRCS} ${VideoFrameGrabberAndViewerWebcamWin_SRCS})
TARGET_LINK_LIBRARIES(VideoFrameGrabberAndViewerWebcamWin IGSTK)
