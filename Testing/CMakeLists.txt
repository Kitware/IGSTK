
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkTests)

SET(IGSTK_TEST_OUTPUT_DIR "${IGSTK_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTK_TEST_OUTPUT_DIR})

SET(IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR "${IGSTK_TEST_OUTPUT_DIR}/StateMachineDiagrams")
MAKE_DIRECTORY(${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})

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

ADD_TEST(igstkLoggerTest ${IGSTK_TESTS} igstkLoggerTest)

ADD_TEST(igstkStateMachineTest ${IGSTK_TESTS} igstkStateMachineTest)

ADD_TEST(igstkStateMachineErrorsTest ${IGSTK_TESTS} igstkStateMachineErrorsTest)

ADD_TEST(igstkTokenTest ${IGSTK_TESTS} igstkTokenTest)

ADD_TEST(igstkBasicTrackerTest ${IGSTK_TESTS} igstkBasicTrackerTest)

ADD_TEST(igstkTrackerToolTest ${IGSTK_TESTS} igstkTrackerToolTest)

ADD_TEST(igstkTrackerPortTest ${IGSTK_TESTS} igstkTrackerPortTest)

ADD_TEST(igstkSceneTest ${IGSTK_TESTS} igstkSceneTest)

ADD_TEST(igstkViewTest ${IGSTK_TESTS} igstkViewTest)

ADD_TEST(igstkTransformTest ${IGSTK_TESTS} igstkTransformTest)

ADD_TEST(igstkTimeStampTest ${IGSTK_TESTS} igstkTimeStampTest)

ADD_TEST(igstkEllipsoidObjectTest ${IGSTK_TESTS} igstkEllipsoidObjectTest)

ADD_TEST(igstkCylinderObjectTest ${IGSTK_TESTS} igstkCylinderObjectTest)

SET(BasicTests_SRCS
  igstkCommunicationTest
  igstkCylinderObjectTest
  igstkEllipsoidObjectTest
  igstkMultipleOutputTest
  igstkLoggerTest
  igstkSceneTest
  igstkStateMachineTest
  igstkStateMachineErrorsTest
  igstkTokenTest
  igstkBasicTrackerTest
  igstkTimeStampTest
  igstkTrackerToolTest
  igstkTrackerPortTest
  igstkTransformTest
  igstkViewTest
  )

ADD_EXECUTABLE(igstkTests igstkTests.cxx ${BasicTests_SRCS})

TARGET_LINK_LIBRARIES(igstkTests IGSTK)

ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)

ADD_EXECUTABLE(igstkStateMachineExportTest igstkStateMachineExportTest.cxx)
ADD_TEST(igstkStateMachineExportTest ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
TARGET_LINK_LIBRARIES(igstkStateMachineExportTest IGSTK)

# Configure a header needed by igstkSystemInformation.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/igstkSystemInformation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/igstkSystemInformation.h"
               @ONLY IMMEDIATE)

