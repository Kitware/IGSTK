#-----------------------------------------------------------------------------
# Set some variables
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkTests)

IF(IGSTK_DATA_ROOT)
  SET(BASELINE ${IGSTK_DATA_ROOT}/Baseline)
ENDIF(IGSTK_DATA_ROOT)

SET(IGSTK_TEST_OUTPUT_DIR "${IGSTK_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTK_TEST_OUTPUT_DIR})

SET(IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR "${IGSTK_TEST_OUTPUT_DIR}/StateMachineDiagrams")
MAKE_DIRECTORY(${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})

SET(IGSTK_DICOM_TEST_OUTPUT_DIR "${IGSTK_TEST_OUTPUT_DIR}/DICOM")
MAKE_DIRECTORY(${IGSTK_DICOM_TEST_OUTPUT_DIR})

#-----------------------------------------------------------------------------
# Options for devices attached to serial port for testing
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
INCLUDE_DIRECTORIES (
  ${IGSTK_SOURCE_DIR}
  ${IGSTK_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Source
  ${IGSTK_BINARY_DIR}/Source
  ${IGSTK_SOURCE_DIR}/Testing
  ${IGSTK_BINARY_DIR}/Testing
  )

#-----------------------------------------------------------------------------
# Add testing command
ADD_TEST(igstkBasicTrackerTest ${IGSTK_TESTS} igstkBasicTrackerTest)
ADD_TEST(igstkBinaryDataTest ${IGSTK_TESTS} igstkBinaryDataTest)
ADD_TEST(igstkCommunicationTest ${IGSTK_TESTS} igstkCommunicationTest)
ADD_TEST(igstkCTImageSpatialObjectTest ${IGSTK_TESTS} igstkCTImageSpatialObjectTest )
ADD_TEST(igstkImageReaderTest ${IGSTK_TESTS} igstkImageReaderTest )
ADD_TEST(igstkImageSpatialObjectTest ${IGSTK_TESTS} igstkImageSpatialObjectTest )
ADD_TEST(igstkLandmark3DRegistrationTest ${IGSTK_TESTS} igstkLandmark3DRegistrationTest)
ADD_TEST(igstkMRImageSpatialObjectRepresentationTest ${IGSTK_TESTS} igstkMRImageSpatialObjectRepresentationTest )
ADD_TEST(igstkMRImageSpatialObjectTest ${IGSTK_TESTS} igstkMRImageSpatialObjectTest )          
ADD_TEST(igstkMultipleOutputTest ${IGSTK_TESTS} igstkMultipleOutputTest)
ADD_TEST(igstkSerialCommunicationTest ${IGSTK_TESTS} igstkSerialCommunicationTest)
ADD_TEST(igstkStateMachineErrorsTest ${IGSTK_TESTS} igstkStateMachineErrorsTest)
ADD_TEST(igstkStateMachineTest ${IGSTK_TESTS} igstkStateMachineTest)
ADD_TEST(igstkStringEventTest ${IGSTK_TESTS} igstkStringEventTest )
ADD_TEST(igstkTimeStampTest ${IGSTK_TESTS} igstkTimeStampTest)
ADD_TEST(igstkTokenTest ${IGSTK_TESTS} igstkTokenTest)
ADD_TEST(igstkTrackerPortTest ${IGSTK_TESTS} igstkTrackerPortTest)
ADD_TEST(igstkTrackerToolTest ${IGSTK_TESTS} igstkTrackerToolTest)
ADD_TEST(igstkTransformTest ${IGSTK_TESTS} igstkTransformTest)
ADD_TEST(igstkVTKLoggerOutputTest ${IGSTK_TESTS} igstkVTKLoggerOutputTest)

#-----------------------------------------------------------------------------
# Simulation test


#-----------------------------------------------------------------------------
# Tests need data input
IF (IGSTK_DATA_ROOT) 

  ADD_TEST(igstkAuroraTrackerSimulatedTest ${IGSTK_TESTS} igstkAuroraTrackerSimulatedTest)
  ADD_TEST(igstkNDICommandInterpreterSimulatedTest ${IGSTK_TESTS} igstkNDICommandInterpreterSimulatedTest)
  ADD_TEST(igstkNDICommandInterpreterStressTest ${IGSTK_TESTS} igstkNDICommandInterpreterStressTest)
  ADD_TEST(igstkPolarisTrackerSimulatedTest ${IGSTK_TESTS} igstkPolarisTrackerSimulatedTest)
  ADD_TEST(igstkSerialCommunicationSimulatorTest ${IGSTK_TESTS} igstkSerialCommunicationSimulatorTest)
  
  ADD_TEST( igstkDICOMImageReaderTest ${IGSTK_TESTS} igstkDICOMImageReaderTest 
       ${IGSTK_DATA_ROOT}/Input/E000192 )
  ADD_TEST( igstkCTImageReaderTest ${IGSTK_TESTS} igstkCTImageReaderTest 
       ${IGSTK_DATA_ROOT}/Input/E000192 )
  ADD_TEST( igstkMRImageReaderTest ${IGSTK_TESTS} igstkMRImageReaderTest 
       ${IGSTK_DATA_ROOT}/Input/E000192 )
  ADD_TEST( igstkMeshReaderTest ${IGSTK_TESTS} igstkMeshReaderTest 
       ${IGSTK_DATA_ROOT}/Input/liver.msh  
       ${IGSTK_DATA_ROOT}/Input/liverCorruptedOnPurpose.msh )
  ADD_TEST( igstkTubeReaderTest ${IGSTK_TESTS} igstkTubeReaderTest 
       ${IGSTK_DATA_ROOT}/Input/vessel.tre 
       ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre  
       ${IGSTK_DATA_ROOT}/Input/vesselWithoutReadPermissions.tre )
  ADD_TEST( igstkSpatialObjectReaderTest ${IGSTK_TESTS} igstkSpatialObjectReaderTest 
       ${IGSTK_DATA_ROOT}/Input/vessel.tre 
       ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre  
       ${IGSTK_DATA_ROOT}/Input/vesselWithoutReadPermissions.tre )       
  ADD_TEST( igstkCTImageSpatialObjectRepresentationTest ${IGSTK_TESTS} 
       igstkCTImageSpatialObjectRepresentationTest
       ${IGSTK_DATA_ROOT}/Input/E000192)          
  ADD_TEST( igstkDICOMImageReaderErrorsTest ${IGSTK_TESTS} igstkDICOMImageReaderErrorsTest 
       ${IGSTK_DATA_ROOT}/Input/E000192Mod ${IGSTK_DICOM_TEST_OUTPUT_DIR})          
ENDIF(IGSTK_DATA_ROOT)

#-----------------------------------------------------------------------------
# Tests depend on external device
IF(IGSTK_TEST_AURORA_ATTACHED)
  ADD_TEST(igstkAuroraTrackerTest ${IGSTK_TESTS} igstkAuroraTrackerTest)
ENDIF(IGSTK_TEST_AURORA_ATTACHED)

IF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)
  ADD_TEST(igstkNDICommandInterpreterTest ${IGSTK_TESTS} igstkNDICommandInterpreterTest)
ENDIF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)

IF(IGSTK_TEST_POLARIS_ATTACHED)
  ADD_TEST(igstkPolarisTrackerTest ${IGSTK_TESTS} igstkPolarisTrackerTest)
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

#-----------------------------------------------------------------------------
# Tests depend on FLTK
IF(IGSTK_USE_FLTK)
  ADD_TEST(igstkCylinderObjectTest ${IGSTK_TESTS} igstkCylinderObjectTest)
  ADD_TEST(igstkEllipsoidObjectTest ${IGSTK_TESTS} igstkEllipsoidObjectTest)
  ADD_TEST(igstkFLTKTextBufferLogOutputTest ${IGSTK_TESTS} igstkFLTKTextBufferLogOutputTest)
  ADD_TEST(igstkFLTKTextLogOutputTest ${IGSTK_TESTS} igstkFLTKTextLogOutputTest)
  ADD_TEST(igstkViewTest ${IGSTK_TESTS} igstkViewTest)
  ADD_TEST(igstkViewRefreshRateTest ${IGSTK_TESTS} igstkViewRefreshRateTest)
  ADD_TEST(igstkPulseGeneratorTest ${IGSTK_TESTS} igstkPulseGeneratorTest)
  ADD_TEST(igstkTubeObjectTest ${IGSTK_TESTS} igstkTubeObjectTest)
  
  IF (IGSTK_DATA_ROOT)
    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest 
              ${IGSTK_TESTS}
              --compare ${BASELINE}/igstkViewScreenShot.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot.png 
              --tolerance 5
              igstkCTImageSpatialObjectReadingAndRepresentationTest
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )
    ADD_TEST( igstkImageSpatialObjectRepresentationTest ${IGSTK_TESTS} 
              igstkImageSpatialObjectRepresentationTest 
              ${IGSTK_DATA_ROOT}/Input/E000192)   
  ENDIF (IGSTK_DATA_ROOT)
            
ENDIF(IGSTK_USE_FLTK)

#-----------------------------------------------------------------------------
# Set the source file
SET(BasicTests_SRCS
  igstkBasicTrackerTest.cxx
  igstkBinaryDataTest.cxx
  igstkCommunicationTest.cxx
  igstkCTImageSpatialObjectTest.cxx
  igstkImageReaderTest.cxx
  igstkImageSpatialObjectTest.cxx
  igstkLandmark3DRegistrationTest.cxx
  igstkMRImageSpatialObjectRepresentationTest.cxx
  igstkMRImageSpatialObjectTest.cxx
  igstkMultipleOutputTest.cxx    
  igstkSerialCommunicationTest.cxx
  igstkStateMachineErrorsTest.cxx
  igstkStateMachineTest.cxx
  igstkStringEventTest.cxx
  igstkTimeStampTest.cxx
  igstkTokenTest.cxx
  igstkTrackerPortTest.cxx
  igstkTrackerToolTest.cxx
  igstkTransformTest.cxx  
  igstkVTKLoggerOutputTest.cxx
  )  
#-----------------------------------------------------------------------------
# Testing source file depend on external device
IF(IGSTK_TEST_AURORA_ATTACHED)
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAuroraTrackerTest.cxx
  )
ENDIF(IGSTK_TEST_AURORA_ATTACHED)

IF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkNDICommandInterpreterTest
  )
ENDIF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)

IF(IGSTK_TEST_POLARIS_ATTACHED)
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkPolarisTrackerTest.cxx
  )
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

#-----------------------------------------------------------------------------
# Testing source file need data input
IF(IGSTK_DATA_ROOT)
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkCTImageReaderTest.cxx
    igstkCTImageSpatialObjectRepresentationTest.cxx
    igstkDICOMImageReaderErrorsTest.cxx
    igstkDICOMImageReaderTest.cxx
    igstkMeshReaderTest.cxx 
    igstkMRImageReaderTest.cxx
    igstkSpatialObjectReaderTest.cxx
    igstkTubeReaderTest.cxx
    
    igstkAuroraTrackerSimulatedTest.cxx
    igstkNDICommandInterpreterSimulatedTest.cxx
    igstkNDICommandInterpreterStressTest.cxx   
    igstkPolarisTrackerSimulatedTest.cxx 
    igstkSerialCommunicationSimulatorTest.cxx
  )  
ENDIF(IGSTK_DATA_ROOT)

#-----------------------------------------------------------------------------
# Testing source file depend on FLTK
IF(IGSTK_USE_FLTK)  
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkCylinderObjectTest.cxx
    igstkEllipsoidObjectTest.cxx
    igstkFLTKTextBufferLogOutputTest.cxx
    igstkFLTKTextLogOutputTest.cxx
    igstkMeshObjectTest.cxx
    igstkPulseGeneratorTest.cxx
    igstkTubeObjectTest.cxx
    igstkViewTest.cxx
    igstkViewRefreshRateTest.cxx
  )    
  IF(IGSTK_DATA_ROOT)
    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx
      igstkImageSpatialObjectRepresentationTest.cxx
    ) 
  ENDIF(IGSTK_DATA_ROOT)  
ENDIF(IGSTK_USE_FLTK)

#-----------------------------------------------------------------------------
# Add testing executables
ADD_EXECUTABLE(igstkTests igstkTests.cxx ${BasicTests_SRCS})

TARGET_LINK_LIBRARIES(igstkTests IGSTK)

ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)

ADD_EXECUTABLE(igstkStateMachineExportTest igstkStateMachineExportTest.cxx)
ADD_TEST(igstkStateMachineExportTest ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
TARGET_LINK_LIBRARIES(igstkStateMachineExportTest IGSTK)

#-----------------------------------------------------------------------------
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
