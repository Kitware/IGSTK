
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkTests)


ADD_TEST(DicomImageReaderTest ${IGSTK_TESTS} DicomImageReaderTest)
ADD_TEST(RegistrationCTTest ${IGSTK_TESTS} RegistrationCTTest)


SET(BasicTests_SRCS
  DicomImageReaderTest.cxx
  RegistrationCTTest.cxx
  )

ADD_EXECUTABLE(igstkTests igstkTests.cxx ${BasicTests_SRCS})
TARGET_LINK_LIBRARIES(igstkTests IGSTK)

