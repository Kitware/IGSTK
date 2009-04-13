PROJECT(CalibrationRegistrationLoading)  
  
#
# Source code for the project
#
SET(SRCS
    igstkPrecomputedTransformData.cxx
    igstkTransformXMLFileReaderBase.cxx
    igstkRigidTransformXMLFileReader.cxx
    igstkAffineTransformXMLFileReader.cxx
    igstkPerspectiveTransformXMLFileReader.cxx
    igstkTransformFileReader.cxx
)
SET(HDRS
    igstkPrecomputedTransformData.h
    igstkTransformXMLFileReaderBase.h
    igstkRigidTransformXMLFileReader.h
    igstkAffineTransformXMLFileReader.h
    igstkPerspectiveTransformXMLFileReader.h
    igstkTransformFileReader.h
)

INCLUDE_DIRECTORIES(
  ${CalibrationRegistrationLoading_SOURCE_DIR}
  ${CalibrationRegistrationLoading_BINARY_DIR}
  )

ADD_LIBRARY(CalibrationRegistrationLoading ${SRCS})
TARGET_LINK_LIBRARIES(CalibrationRegistrationLoading IGSTKSandbox)


