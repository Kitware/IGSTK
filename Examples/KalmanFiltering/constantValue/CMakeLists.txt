PROJECT(KF_ConstantValueExample)
  
SET(KF_ConstantValueExample_SRCS
  KF_ConstantValueExample.cpp
  )
  
INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_BINARY_DIR}
  ${KF_ConstantValueExample_SOURCE_DIR}
  ${KF_ConstantValueExample_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/KalmanFiltering/constantValue
  )

ADD_EXECUTABLE(KF_ConstantValueExample  ${KF_ConstantValueExample_SRCS})
TARGET_LINK_LIBRARIES(KF_ConstantValueExample IGSTKSandbox )
