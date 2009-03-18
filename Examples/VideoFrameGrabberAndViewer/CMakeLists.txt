PROJECT(VideoFrameGrabberAndViewer)

INCLUDE_DIRECTORIES (
 /usr/local/include/unicap
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration
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

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkImagingSourceImagerConfiguration.cxx
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkTerasonImagerConfiguration.cxx
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkCompressedDVImagerConfiguration.cxx
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkImagerConfiguration.cxx
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkImagerController.cxx

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkImagerController.h
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkImagerConfiguration.h
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkImagingSourceImagerConfiguration.h
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkTerasonImagerConfiguration.h
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/ImagerConfiguration/igstkCompressedDVImagerConfiguration.h

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
