
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkTests)

SET(IGSTK_TEST_OUTPUT_DIR "${IGSTK_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTK_TEST_OUTPUT_DIR})

#-----------------------------------------------------------------------------
# Configure the default IGSTK_DATA_ROOT for the location of IGSTK Data.
FIND_PATH(IGSTK_DATA_ROOT IGSTKData.readme ${IGSTK_SOURCE_DIR}/Testing/Data $ENV{IGSTK_DATA_ROOT})
MARK_AS_ADVANCED(IGSTK_DATA_ROOT)

INCLUDE_DIRECTORIES (
  ${IGSTK_SOURCE_DIR}
  ${IGSTK_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Source
  ${IGSTK_BINARY_DIR}/Source
  ${IGSTK_SOURCE_DIR}/Testing
  ${IGSTK_BINARY_DIR}/Testing
  )

ADD_TEST(igstkCommunicationTest ${IGSTK_TESTS} igstkCommunicationTest)

ADD_TEST(igstkMultipleOutputTest ${IGSTK_TESTS} igstkMultipleOutputTest)

ADD_TEST(igstkStateMachineTest ${IGSTK_TESTS} igstkStateMachineTest)

ADD_TEST(igstkStateMachineErrorsTest ${IGSTK_TESTS} igstkStateMachineErrorsTest)

ADD_TEST(igstkStateMachineTokenTest ${IGSTK_TESTS} igstkStateMachineTokenTest)

ADD_TEST(igstkBasicTrackerTest ${IGSTK_TESTS} igstkBasicTrackerTest)

ADD_TEST(igstkTrackerToolTest ${IGSTK_TESTS} igstkTrackerToolTest)

ADD_TEST(igstkTrackerPortTest ${IGSTK_TESTS} igstkTrackerPortTest)

SET(BasicTests_SRCS
  igstkCommunicationTest
  igstkMultipleOutputTest
  igstkStateMachineTest
  igstkStateMachineErrorsTest
  igstkStateMachineTokenTest
  igstkBasicTrackerTest
  igstkTrackerToolTest
  igstkTrackerPortTest
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

