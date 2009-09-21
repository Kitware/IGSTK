PROJECT(VideoFrameGrabberAndViewerMicron)
 
SET(VideoFrameGrabberAndViewerMicron_GUI_SRCS
    VideoFrameGrabberAndViewerMicronGUI.fl
  )
  
SET(VideoFrameGrabberAndViewerMicron_SRCS
  main.cxx
  VideoFrameGrabberAndViewerMicron.h
  VideoFrameGrabberAndViewerMicron.cxx
  VideoFrameGrabberAndViewerMicronView.h
  VideoFrameGrabberAndViewerMicronView.cxx
  )
  
INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_BINARY_DIR}
  ${VideoFrameGrabberAndViewerMicron_SOURCE_DIR}
  ${VideoFrameGrabberAndViewerMicron_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/VideoFrameGrabberAndViewerMicron
  )

FLTK_WRAP_UI(VideoFrameGrabberAndViewerMicron ${VideoFrameGrabberAndViewerMicron_GUI_SRCS})
ADD_EXECUTABLE(VideoFrameGrabberAndViewerMicron ${VideoFrameGrabberAndViewerMicron_FLTK_UI_SRCS} ${VideoFrameGrabberAndViewerMicron_SRCS})
TARGET_LINK_LIBRARIES(VideoFrameGrabberAndViewerMicron IGSTKSandbox )
