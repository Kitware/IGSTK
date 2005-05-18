
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
ADD_TEST(igstkAtamaiNDITrackerTest ${IGSTKSandbox_TESTS} igstkAtamaiNDITrackerTest)
ADD_TEST(igstkNDICyclicRedundancyTest ${IGSTKSandbox_TESTS} igstkNDICyclicRedundancyTest)

ADD_TEST(itkRealtimeClockTest ${IGSTKSandbox_TESTS} itkRealtimeClockTest)
ADD_TEST(itkStdStreamLogOutputTest ${IGSTKSandbox_TESTS} itkStdStreamLogOutputTest)
ADD_TEST(itkMultipleLogOutputTest ${IGSTKSandbox_TESTS} itkMultipleLogOutputTest)
ADD_TEST(itkLoggerTest ${IGSTKSandbox_TESTS} itkLoggerTest)
ADD_TEST(itkThreadLoggerTest ${IGSTKSandbox_TESTS} itkThreadLoggerTest)
ADD_TEST(itkLoggerOutputTest ${IGSTKSandbox_TESTS} itkLoggerOutputTest)
ADD_TEST(itkLoggerManagerTest ${IGSTKSandbox_TESTS} itkLoggerManagerTest)
ADD_TEST(igstkVTKLoggerOutputTest ${IGSTKSandbox_TESTS} igstkVTKLoggerOutputTest)
ADD_TEST(igstkFLTKTextLogOutputTest ${IGSTKSandbox_TESTS} igstkFLTKTextLogOutputTest)
ADD_TEST(igstkFLTKTextBufferLogOutputTest ${IGSTKSandbox_TESTS} igstkFLTKTextBufferLogOutputTest)


SET(BasicTests_SRCS
  igstkMouseTrackerTest.cxx
  igstkSerialCommunicationTest.cxx
  igstkNDICommandInterpreterTest.cxx
  igstkAtamaiNDITrackerTest.cxx
  igstkNDICyclicRedundancyTest.cxx
  itkRealtimeClockTest.cxx
  itkStdStreamLogOutputTest.cxx
  itkMultipleLogOutputTest.cxx
  itkLoggerTest.cxx
  itkThreadLoggerTest.cxx
  itkLoggerOutputTest.cxx
  itkLoggerManagerTest.cxx
  igstkVTKLoggerOutputTest.cxx
  igstkFLTKTextLogOutputTest.cxx
  igstkFLTKTextBufferLogOutputTest.cxx
  )



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

