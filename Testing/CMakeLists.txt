
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkTests)

SET(IGSTK_TEST_OUTPUT_DIR "${IGSTK_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTK_TEST_OUTPUT_DIR})

#-----------------------------------------------------------------------------
# Configure the default IGSTK_DATA_ROOT for the location of IGSTK Data.
FIND_PATH(IGSTK_DATA_ROOT IGSTKData.readme ${IGSTK_SOURCE_DIR}/Testing/Data $ENV{IGSTK_DATA_ROOT})
MARK_AS_ADVANCED(IGSTK_DATA_ROOT)


ADD_TEST(igstkStateMachineTest ${IGSTK_TESTS} igstkStateMachineTest)

SET(BasicTests_SRCS
  igstkStateMachineTest
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

