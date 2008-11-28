PROJECT(ImagerConfiguration)  
  
SET(ImagerConfiguration_SRCS
  igstkImagerConfiguration.cxx
  igstkImagerController.cxx
)

IF(IGSTKSandbox_USE_OpenIGTLink)
  SET(ImagerConfiguration_SRCS ${ImagerConfiguration_SRCS}
     igstkTerasonImagerConfiguration.cxx
     )
ENDIF(IGSTKSandbox_USE_OpenIGTLink)
  
INCLUDE_DIRECTORIES(
  ${ImagerConfiguration_SOURCE_DIR}
  ${ImagerConfiguration_BINARY_DIR}
  )


