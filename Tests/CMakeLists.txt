
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkTests)


ADD_TEST(DicomImageReaderTest ${IGSTK_TESTS} DicomImageReaderTest)
ADD_TEST(RegistrationCTTest ${IGSTK_TESTS} RegistrationCTTest)


SET(BasicTests_SRCS
  DicomImageReaderTest.cxx
  RegistrationCTTest.cxx
  )

ADD_EXECUTABLE(igstkTests igstkTests.cxx ${BasicTests_SRCS})
TARGET_LINK_LIBRARIES(igstkTests IGSTK)

ADD_EXECUTABLE(SystemInformation SystemInformation.cxx)
ADD_TEST(SystemInformation ${EXECUTABLE_OUTPUT_PATH}/SystemInformation)
TARGET_LINK_LIBRARIES(SystemInformation vtkCommon)

# Configure a header needed by SystemInformation.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SystemInformation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SystemInformation.h"
               @ONLY IMMEDIATE)

