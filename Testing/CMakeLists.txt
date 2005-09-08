
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkTests)

SET(IGSTK_TEST_OUTPUT_DIR "${IGSTK_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTK_TEST_OUTPUT_DIR})

SET(IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR "${IGSTK_TEST_OUTPUT_DIR}/StateMachineDiagrams")
MAKE_DIRECTORY(${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})

# options for devices attached to serial port for testing
OPTION(IGSTK_TEST_AURORA_ATTACHED "Aurora is attached to computer" OFF)
SET( IGSTK_TEST_AURORA_PORT_NUMBER 0 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
OPTION(IGSTK_TEST_POLARIS_ATTACHED "Polaris is attached to computer" OFF)
SET( IGSTK_TEST_POLARIS_PORT_NUMBER 0 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
OPTION(IGSTK_TEST_LOOPBACK_ATTACHED "Serial Loopback is attached" OFF)
SET( IGSTK_TEST_LOOPBACK_PORT_NUMBER 1 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )

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

ADD_TEST(igstkAuroraTrackerTest ${IGSTK_TESTS} igstkAuroraTrackerTest)

ADD_TEST(igstkBasicTrackerTest ${IGSTK_TESTS} igstkBasicTrackerTest)

ADD_TEST(igstkBinaryDataTest ${IGSTK_TESTS} igstkBinaryDataTest)

ADD_TEST(igstkCommunicationTest ${IGSTK_TESTS} igstkCommunicationTest)

ADD_TEST(igstkCylinderObjectTest ${IGSTK_TESTS} igstkCylinderObjectTest)

ADD_TEST(igstkEllipsoidObjectTest ${IGSTK_TESTS} igstkEllipsoidObjectTest)

ADD_TEST(igstkFLTKTextBufferLogOutputTest ${IGSTK_TESTS} igstkFLTKTextBufferLogOutputTest)

ADD_TEST(igstkFLTKTextLogOutputTest ${IGSTK_TESTS} igstkFLTKTextLogOutputTest)

ADD_TEST(igstkMeshObjectTest ${IGSTK_TESTS} igstkMeshObjectTest)

ADD_TEST(igstkMultipleOutputTest ${IGSTK_TESTS} igstkMultipleOutputTest)

ADD_TEST(igstkNDICommandInterpreterTest ${IGSTK_TESTS} igstkNDICommandInterpreterTest)

ADD_TEST(igstkNDICommandInterpreterStressTest ${IGSTK_TESTS} igstkNDICommandInterpreterStressTest)

ADD_TEST(igstkPulseGeneratorTest ${IGSTK_TESTS} igstkPulseGeneratorTest)

ADD_TEST(igstkSerialCommunicationTest ${IGSTK_TESTS} igstkSerialCommunicationTest)

ADD_TEST(igstkSerialCommunicationSimulatorTest ${IGSTK_TESTS} igstkSerialCommunicationSimulatorTest)

ADD_TEST(igstkStateMachineTest ${IGSTK_TESTS} igstkStateMachineTest)

ADD_TEST(igstkStateMachineErrorsTest ${IGSTK_TESTS} igstkStateMachineErrorsTest)

ADD_TEST(igstkTimeStampTest ${IGSTK_TESTS} igstkTimeStampTest)

ADD_TEST(igstkTokenTest ${IGSTK_TESTS} igstkTokenTest)

ADD_TEST(igstkTrackerToolTest ${IGSTK_TESTS} igstkTrackerToolTest)

ADD_TEST(igstkTrackerPortTest ${IGSTK_TESTS} igstkTrackerPortTest)

ADD_TEST(igstkTransformTest ${IGSTK_TESTS} igstkTransformTest)

ADD_TEST(igstkTubeObjectTest ${IGSTK_TESTS} igstkTubeObjectTest)

ADD_TEST(igstkViewTest ${IGSTK_TESTS} igstkViewTest)

ADD_TEST(igstkVTKLoggerOutputTest ${IGSTK_TESTS} igstkVTKLoggerOutputTest)


SET(BasicTests_SRCS
  igstkAuroraTrackerTest
  igstkBasicTrackerTest
  igstkBinaryDataTest
  igstkCommunicationTest
  igstkCylinderObjectTest
  igstkEllipsoidObjectTest
  igstkFLTKTextBufferLogOutputTest
  igstkFLTKTextLogOutputTest
  igstkMeshObjectTest
  igstkMultipleOutputTest
  igstkNDICommandInterpreterTest
  igstkNDICommandInterpreterStressTest
  igstkPulseGeneratorTest
  igstkSerialCommunicationTest
  igstkSerialCommunicationSimulatorTest
  igstkStateMachineTest
  igstkStateMachineErrorsTest
  igstkTimeStampTest
  igstkTokenTest
  igstkTrackerPortTest
  igstkTrackerToolTest
  igstkTransformTest
  igstkTubeObjectTest
  igstkViewTest
  igstkVTKLoggerOutputTest
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

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/Input/polaris_stream_07_27_2005.bin"
               "${CXX_TEST_PATH}/polaris_stream_07_27_2005.bin"
               COPYONLY @ONLY IMMEDIATE)

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/Input/polaris_stream_07_27_2005.bin"
               "${CMAKE_CURRENT_BINARY_DIR}/polaris_stream_07_27_2005.bin"
               COPYONLY @ONLY IMMEDIATE)
