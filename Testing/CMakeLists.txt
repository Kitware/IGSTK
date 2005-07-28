
SET(IGSTKSandbox_TESTS ${CXX_TEST_PATH}/igstkSandboxTests)

SET(IGSTKSandbox_TEST_OUTPUT_DIR "${IGSTKSandbox_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTKSandbox_TEST_OUTPUT_DIR})

SET(IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR "${IGSTKSandbox_TEST_OUTPUT_DIR}/StateMachineDiagrams")
MAKE_DIRECTORY(${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})

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

ADD_TEST(igstkMouseTrackerTest ${IGSTKSandbox_TESTS} igstkMouseTrackerTest)
ADD_TEST(igstkSerialCommunicationTest ${IGSTKSandbox_TESTS} igstkSerialCommunicationTest)
ADD_TEST(igstkNDICommandInterpreterTest ${IGSTKSandbox_TESTS} igstkNDICommandInterpreterTest)
ADD_TEST(igstkAuroraTrackerTest ${IGSTKSandbox_TESTS} igstkAuroraTrackerTest)

ADD_TEST(igstkVTKLoggerOutputTest ${IGSTKSandbox_TESTS} igstkVTKLoggerOutputTest)
ADD_TEST(igstkFLTKTextLogOutputTest ${IGSTKSandbox_TESTS} igstkFLTKTextLogOutputTest)
ADD_TEST(igstkFLTKTextBufferLogOutputTest ${IGSTKSandbox_TESTS} igstkFLTKTextBufferLogOutputTest)
ADD_TEST(igstkSerialCommunicationSimulatorTest ${IGSTKSandbox_TESTS} igstkSerialCommunicationSimulatorTest)
ADD_TEST(igstkTubeObjectTest ${IGSTKSandbox_TESTS} igstkTubeObjectTest)
ADD_TEST(igstkMeshObjectTest ${IGSTKSandbox_TESTS} igstkMeshObjectTest)
ADD_TEST(igstkBinaryDataTest ${IGSTKSandbox_TESTS} igstkBinaryDataTest)

SET(BasicTests_SRCS
  igstkMouseTrackerTest.cxx
  igstkSerialCommunicationTest.cxx
  igstkSerialCommunicationSimulatorTest.cxx
  igstkNDICommandInterpreterTest.cxx
  igstkAuroraTrackerTest.cxx
  igstkVTKLoggerOutputTest.cxx
  igstkFLTKTextLogOutputTest.cxx
  igstkFLTKTextBufferLogOutputTest.cxx
  igstkTubeObjectTest.cxx
  igstkMeshObjectTest.cxx
  igstkBinaryDataTest.cxx
  )

IF (IGSTK_USE_ATAMAI)
  ADD_TEST(igstkAtamaiNDITrackerTest ${IGSTKSandbox_TESTS} igstkAtamaiNDITrackerTest)
  SET(BasicTests_SRCS ${BasicTests_SRCS} igstkAtamaiNDITrackerTest)
ENDIF (IGSTK_USE_ATAMAI)


ADD_EXECUTABLE(igstkSandboxTests igstkSandboxTests.cxx ${BasicTests_SRCS})

TARGET_LINK_LIBRARIES(igstkSandboxTests IGSTKSandbox)

ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)


ADD_EXECUTABLE(igstkStateMachineExportTest igstkStateMachineExportTest.cxx)
ADD_TEST(igstkStateMachineExportTest ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
TARGET_LINK_LIBRARIES(igstkStateMachineExportTest IGSTKSandbox)

# Configure a header needed by SystemInformation.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/igstkSystemInformation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/igstkSystemInformation.h"
               @ONLY IMMEDIATE)

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/polaris_stream_07_27_2005.bin"
               "${CXX_TEST_PATH}/polaris_stream_07_27_2005.bin"
               COPYONLY @ONLY IMMEDIATE)

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/polaris_stream_07_27_2005.bin"
               "${CMAKE_CURRENT_BINARY_DIR}/polaris_stream_07_27_2005.bin"
               COPYONLY @ONLY IMMEDIATE)
