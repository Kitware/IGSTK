PROJECT(VideoFrameGrabberAndViewer)

INCLUDE_DIRECTORIES (
 /usr/local/include/unicap
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration
) 
 
SET(VideoFrameGrabberAndViewer_GUI_SRCS
    VideoFrameGrabberAndViewerGUI.fl
  )
  
SET(VideoFrameGrabberAndViewer_SRCS
  main.cxx
  VideoFrameGrabberAndViewer.h
  VideoFrameGrabberAndViewer.cxx
  VideoFrameGrabberAndViewerQuadrantViews.h
  VideoFrameGrabberAndViewerQuadrantViews.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkPrecomputedTransformData.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkTransformXMLFileReaderBase.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkRigidTransformXMLFileReader.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkTransformFileReader.cxx

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkImagingSourceVideoImagerConfiguration.cxx
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkTerasonVideoImagerConfiguration.cxx
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkCompressedDVVideoImagerConfiguration.cxx
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkVideoImagerConfiguration.cxx
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkVideoImagerController.cxx

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkVideoImagerController.h
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkVideoImagerConfiguration.h
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkImagingSourceVideoImagerConfiguration.h
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkTerasonVideoImagerConfiguration.h
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/VideoImagerConfiguration/igstkCompressedDVVideoImagerConfiguration.h

  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkPrecomputedTransformData.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkTransformXMLFileReaderBase.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkRigidTransformXMLFileReader.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkTransformFileReader.h
  )
  
INCLUDE_DIRECTORIES(
  ${VideoFrameGrabberAndViewer_SOURCE_DIR}
  ${VideoFrameGrabberAndViewer_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
   /usr/local/include/unicap
  )

FLTK_WRAP_UI(VideoFrameGrabberAndViewer ${VideoFrameGrabberAndViewer_GUI_SRCS})
ADD_EXECUTABLE(VideoFrameGrabberAndViewer ${VideoFrameGrabberAndViewer_FLTK_UI_SRCS} ${VideoFrameGrabberAndViewer_SRCS})
TARGET_LINK_LIBRARIES(VideoFrameGrabberAndViewer IGSTKSandbox )
