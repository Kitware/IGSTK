
SET(IGSTKSandbox_TESTS ${CXX_TEST_PATH}/igstkSandboxTests)

SET(IGSTKSandbox_TEST_OUTPUT_DIR "${IGSTKSandbox_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTKSandbox_TEST_OUTPUT_DIR})

#-----------------------------------------------------------------------------
# Configure the default IGSTKSandbox_DATA_ROOT for the location of IGSTKSandbox Data.
FIND_PATH(IGSTKSandbox_DATA_ROOT IGSTKSandboxData.readme ${IGSTKSandbox_SOURCE_DIR}/Testing/Data $ENV{IGSTKSandbox_DATA_ROOT})
MARK_AS_ADVANCED(IGSTKSandbox_DATA_ROOT)

INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_SOURCE_DIR}
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/Source
  ${IGSTKSandbox_SOURCE_DIR}/Testing
  ${IGSTKSandbox_BINARY_DIR}/Testing
  )

ADD_TEST(igstkEllipsoidObjectTest ${IGSTKSandbox_TESTS} igstkEllipsoidObjectTest)
ADD_TEST(igstkCylinderObjectTest ${IGSTKSandbox_TESTS} igstkCylinderObjectTest)
ADD_TEST(igstkSceneTest ${IGSTKSandbox_TESTS} igstkSceneTest)
ADD_TEST(igstkViewTest ${IGSTKSandbox_TESTS} igstkViewTest)
ADD_TEST(igstkMouseTrackerTest ${IGSTKSandbox_TESTS} igstkMouseTrackerTest)
ADD_TEST(igstkSerialCommunicationTest ${IGSTKSandbox_TESTS} igstkSerialCommunicationTest)
ADD_TEST(igstkTimeStampTest ${IGSTKSandbox_TESTS} igstkTimeStampTest)
ADD_TEST(igstkTransformTest ${IGSTKSandbox_TESTS} igstkTransformTest)

SET(BasicTests_SRCS
  igstkEllipsoidObjectTest.cxx
  igstkCylinderObjectTest.cxx
  igstkSceneTest.cxx
  igstkViewTest.cxx
  igstkMouseTrackerTest.cxx
  igstkSerialCommunicationTest.cxx
  igstkTimeStampTest.cxx
  igstkTransformTest.cxx
  )



ADD_EXECUTABLE(igstkSandboxTests igstkSandboxTests.cxx ${BasicTests_SRCS})

TARGET_LINK_LIBRARIES(igstkSandboxTests IGSTKSandbox)

ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)


# Configure a header needed by SystemInformation.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/igstkSystemInformation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/igstkSystemInformation.h"
               @ONLY IMMEDIATE)

