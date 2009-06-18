PROJECT(VideoFrameGrabberAndViewerOpenIGTLink)
 
SET(VideoFrameGrabberAndViewerOpenIGTLink_GUI_SRCS
    VideoFrameGrabberAndViewerOpenIGTLinkGUI.fl
  )
  
SET(VideoFrameGrabberAndViewerOpenIGTLink_SRCS
  main.cxx
  VideoFrameGrabberAndViewerOpenIGTLink.h
  VideoFrameGrabberAndViewerOpenIGTLink.cxx
  VideoFrameGrabberAndViewerOpenIGTLinkView.h
  VideoFrameGrabberAndViewerOpenIGTLinkView.cxx
  )
  
INCLUDE_DIRECTORIES(
  ${IGSTK_BINARY_DIR}
  ${VideoFrameGrabberAndViewerOpenIGTLink_SOURCE_DIR}
  ${VideoFrameGrabberAndViewerOpenIGTLink_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Examples/VideoFrameGrabberAndViewerOpenIGTLink
  )

FLTK_WRAP_UI(VideoFrameGrabberAndViewerOpenIGTLink ${VideoFrameGrabberAndViewerOpenIGTLink_GUI_SRCS})
ADD_EXECUTABLE(VideoFrameGrabberAndViewerOpenIGTLink ${VideoFrameGrabberAndViewerOpenIGTLink_FLTK_UI_SRCS} ${VideoFrameGrabberAndViewerOpenIGTLink_SRCS})
TARGET_LINK_LIBRARIES(VideoFrameGrabberAndViewerOpenIGTLink IGSTK)
