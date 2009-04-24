PROJECT(VideoFrameGrabberAndViewerCompressedDV)
 
SET(VideoFrameGrabberAndViewerCompressedDV_GUI_SRCS
    VideoFrameGrabberAndViewerCompressedDVGUI.fl
  )
  
SET(VideoFrameGrabberAndViewerCompressedDV_SRCS
  main.cxx
  VideoFrameGrabberAndViewerCompressedDV.h
  VideoFrameGrabberAndViewerCompressedDV.cxx
  VideoFrameGrabberAndViewerCompressedDVView.h
  VideoFrameGrabberAndViewerCompressedDVView.cxx
  )
  
INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_BINARY_DIR}
  ${VideoFrameGrabberAndViewerCompressedDV_SOURCE_DIR}
  ${VideoFrameGrabberAndViewerCompressedDV_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/VideoFrameGrabberAndViewerCompressedDV
  )

FLTK_WRAP_UI(VideoFrameGrabberAndViewerCompressedDV ${VideoFrameGrabberAndViewerCompressedDV_GUI_SRCS})
ADD_EXECUTABLE(VideoFrameGrabberAndViewerCompressedDV ${VideoFrameGrabberAndViewerCompressedDV_FLTK_UI_SRCS} ${VideoFrameGrabberAndViewerCompressedDV_SRCS})
TARGET_LINK_LIBRARIES(VideoFrameGrabberAndViewerCompressedDV IGSTKSandbox )
