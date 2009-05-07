PROJECT(VideoImagerConfiguration)  
  
SET(VideoImagerConfiguration_SRCS
  igstkVideoImagerConfiguration.cxx
  igstkVideoImagerController.cxx
)

IF(IGSTKSandbox_USE_OpenIGTLink)
  SET(VideoImagerConfiguration_SRCS ${VideoImagerConfiguration_SRCS}
     igstkOpenIGTLinkVideoImagerConfiguration.cxx
     )
ENDIF(IGSTKSandbox_USE_OpenIGTLink)
  
INCLUDE_DIRECTORIES(
  ${VideoImagerConfiguration_SOURCE_DIR}
  ${VideoImagerConfiguration_BINARY_DIR}
  )


