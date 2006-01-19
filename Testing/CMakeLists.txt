
SET(IGSTKSandbox_TESTS ${CXX_TEST_PATH}/igstkSandboxTests)


SET(IGSTKSandbox_TEST_OUTPUT_DIR "${IGSTKSandbox_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTKSandbox_TEST_OUTPUT_DIR})

SET(IGSTK_TEST_FILES_COMBINED ${IGSTKSandbox_BINARY_DIR}/TestingCombined)
MAKE_DIRECTORY( ${IGSTK_TEST_FILES_COMBINED} )

# set the IGSTK output directory to sandbox test directory
SET(IGSTK_TEST_OUTPUT_DIR ${IGSTKSandbox_TEST_OUTPUT_DIR})

SET(IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR "${IGSTKSandbox_TEST_OUTPUT_DIR}/StateMachineDiagrams")
MAKE_DIRECTORY(${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})

SET(IGSTK_DICOM_TEST_OUTPUT_DIR "${IGSTKSandbox_TEST_OUTPUT_DIR}/DICOM")
MAKE_DIRECTORY(${IGSTK_DICOM_TEST_OUTPUT_DIR})


#-----------------------------------------------------------------------------
# Configure the default IGSTKSandbox_DATA_ROOT for the location of IGSTKSandbox Data.
FIND_PATH(IGSTKSandbox_DATA_ROOT IGSTKSandboxData.readme ${IGSTKSandbox_SOURCE_DIR}/Testing/Data $ENV{IGSTKSandbox_DATA_ROOT})

#-----------------------------------------------------------------------------
# Configure the dot tool from graphviz in order to generate the State Machine Diagrams 
FIND_PROGRAM(DOT
  dot
  "C:/Program Files/ATT/Graphviz/bin"
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\ATT\\Graphviz;InstallPath]/bin
)
IF(NOT DOT_PATH)
  GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
ENDIF(NOT DOT_PATH)

FILE(GLOB STATE_MACHINE_DOT_FILES "${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR}/*.dot")

  FOREACH(dotfile ${STATE_MACHINE_DOT_FILES})
    GET_FILENAME_COMPONENT(Imagebase ${dotfile} NAME)
    GET_FILENAME_COMPONENT(ImageBase ${Imagebase} NAME_WE)
    GET_FILENAME_COMPONENT(ImagePath ${dotfile} PATH)
    SET(pngfile ${ImagePath}/${ImageBase}.png)
    ADD_CUSTOM_COMMAND( 
    SOURCE    ${dotfile}
    COMMAND   ${DOT}
    ARGS      -Tpng 
              -o ${pngfile}
              ${dotfile}
    TARGET    StateMachineDiagrams
    OUTPUTS   ${pngfile}
    )
    SET(DOT_PNG_DEPS ${DOT_PNG_DEPS} ${pngfile})
  ENDFOREACH(dotfile)

  ADD_CUSTOM_TARGET(StateMachineDiagrams ALL DEPENDS ${DOT_PNG_DEPS})


#-----------------------------------------------------------------------------
MARK_AS_ADVANCED(IGSTKSandbox_DATA_ROOT)

IF(IGSTKSandbox_DATA_ROOT)
  SET(BASELINE ${IGSTKSandbox_DATA_ROOT}/Baseline)
ENDIF(IGSTKSandbox_DATA_ROOT)


INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_SOURCE_DIR}/SourceLatest
  ${IGSTKSandbox_SOURCE_DIR}
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/Source
  ${IGSTKSandbox_SOURCE_DIR}/Testing
  ${IGSTKSandbox_BINARY_DIR}/Testing
 )

SET(IGSTK_TEST_PROGAMS
  igstkBasicTrackerTest 
  igstkBinaryDataTest 
  igstkCommunicationTest 
  igstkCTImageSpatialObjectTest 
  igstkImageReaderTest 
  igstkImageSpatialObjectTest 
  igstkLandmark3DRegistrationTest 
  igstkMRImageSpatialObjectRepresentationTest 
  igstkMRImageSpatialObjectTest 
  igstkMultipleOutputTest 
  igstkSerialCommunicationTest 
  igstkStateMachineErrorsTest 
  igstkStateMachineTest 
  igstkStringEventTest 
  igstkTimeStampTest 
  igstkTokenTest 
  igstkTrackerPortTest 
  igstkTrackerToolTest
  igstkTransformTest 
  igstkVTKLoggerOutputTest 
  igstkMouseTrackerTest 
  igstkPivotCalibrationTest 
  igstkPrincipalAxisCalibrationTest 
  igstkLandmark3DRegistrationErrorEstimatorTest 
  igstkBoxObjectTest 
  igstkConeObjectTest
  igstkAxesObjectTest
  igstkUltrasoundProbeObjectTest
)

FOREACH(TestProgram ${IGSTK_TEST_PROGAMS})
  ADD_TEST(${TestProgram} ${IGSTKSandbox_TESTS} ${TestProgram})
ENDFOREACH(TestProgram)


IF (IGSTKSandbox_DATA_ROOT) 
  ADD_TEST(igstkAuroraTrackerSimulatedTest ${IGSTKSandbox_TESTS} 
           igstkAuroraTrackerSimulatedTest)
  ADD_TEST(igstkNDICommandInterpreterSimulatedTest ${IGSTKSandbox_TESTS} 
           igstkNDICommandInterpreterSimulatedTest)
  ADD_TEST(igstkNDICommandInterpreterStressTest ${IGSTKSandbox_TESTS} 
           igstkNDICommandInterpreterStressTest)
  ADD_TEST(igstkPolarisTrackerSimulatedTest ${IGSTKSandbox_TESTS} 
           igstkPolarisTrackerSimulatedTest)
  ADD_TEST(igstkSerialCommunicationSimulatorTest ${IGSTKSandbox_TESTS} 
           igstkSerialCommunicationSimulatorTest)
  ADD_TEST( igstkDICOMImageReaderTest ${IGSTKSandbox_TESTS} 
           igstkDICOMImageReaderTest  ${IGSTK_DATA_ROOT}/Input/E000192 )
  ADD_TEST( igstkCTImageReaderTest ${IGSTKSandbox_TESTS} igstkCTImageReaderTest 
           ${IGSTK_DATA_ROOT}/Input/E000192 )
  ADD_TEST( igstkMRImageReaderTest ${IGSTKSandbox_TESTS} igstkMRImageReaderTest 
           ${IGSTK_DATA_ROOT}/Input/E000192 )
  ADD_TEST( igstkMeshReaderTest ${IGSTKSandbox_TESTS} igstkMeshReaderTest 
           ${IGSTK_DATA_ROOT}/Input/liver.msh  
           ${IGSTK_DATA_ROOT}/Input/liverCorruptedOnPurpose.msh )
  ADD_TEST( igstkTubeReaderTest ${IGSTKSandbox_TESTS} igstkTubeReaderTest 
           ${IGSTK_DATA_ROOT}/Input/vessel.tre 
           ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre  
           ${IGSTK_DATA_ROOT}/Input/vesselWithoutReadPermissions.tre )
  ADD_TEST( igstkSpatialObjectReaderTest ${IGSTKSandbox_TESTS} igstkSpatialObjectReaderTest 
          ${IGSTK_DATA_ROOT}/Input/vessel.tre 
          ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre  
          ${IGSTK_DATA_ROOT}/Input/vesselWithoutReadPermissions.tre )       
  ADD_TEST( igstkCTImageSpatialObjectRepresentationTest ${IGSTKSandbox_TESTS} 
       igstkCTImageSpatialObjectRepresentationTest
          ${IGSTK_DATA_ROOT}/Input/E000192)          
  ADD_TEST( igstkDICOMImageReaderErrorsTest ${IGSTKSandbox_TESTS} igstkDICOMImageReaderErrorsTest 
          ${IGSTK_DATA_ROOT}/Input/E000192Mod ${IGSTK_DICOM_TEST_OUTPUT_DIR})          
  ADD_TEST( igstkAnnotation2DTest ${IGSTKSandbox_TESTS} igstkAnnotation2DTest
          ${IGSTKSandbox_DATA_ROOT}/Input/E000192 )
ENDIF(IGSTKSandbox_DATA_ROOT)

# Set the source file
SET(IGSTK_TEST_SRCS
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
  igstkMouseTrackerTest.cxx
  igstkPivotCalibrationTest.cxx
  igstkPrincipalAxisCalibrationTest.cxx
  igstkLandmark3DRegistrationErrorEstimatorTest.cxx
  igstkAxesObjectTest.cxx
  igstkConeObjectTest.cxx
  igstkBoxObjectTest.cxx
  igstkAnnotation2DTest.cxx
  igstkUltrasoundProbeObjectTest.cxx
  )  

# Testing source file need data input
IF(IGSTK_DATA_ROOT)
  SET(IGSTK_TEST_SRCS
    ${IGSTK_TEST_SRCS}
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

IF(IGSTK_USE_FLTK)  
  SET(IGSTK_TEST_SRCS
    ${IGSTK_TEST_SRCS}
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
    SET(IGSTK_TEST_SRCS
      ${IGSTK_TEST_SRCS}
      igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx
      igstkImageSpatialObjectRepresentationTest.cxx
    ) 
  ENDIF(IGSTK_DATA_ROOT)  
ENDIF(IGSTK_USE_FLTK)

SET(BasicTests_SRCS)
FOREACH(SourceFile ${IGSTK_TEST_SRCS})
  IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Testing/${SourceFile})
    SET(TestFullPath ${IGSTKSandbox_SOURCE_DIR}/Testing/${SourceFile})
  ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Testing/${SourceFile})
    SET(TestFullPath ${IGSTK_SOURCE_DIR}/Testing/${SourceFile})
  ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Testing/${SourceFile})

  CONFIGURE_FILE( ${TestFullPath} ${IGSTK_TEST_FILES_COMBINED} COPYONLY )
  SET(BasicTests_SRCS ${BasicTests_SRCS} ${IGSTK_TEST_FILES_COMBINED}/${SourceFile})
ENDFOREACH( SourceFile )


#-----------------------------------------------------------------------------
# Testing source file depend on external device

SET( IGSTK_TEST_DEPEND_ON_DEVICES_SRCS 
     igstkAuroraTrackerTest.cxx
     igstkNDICommandInterpreterTest.cxx
     igstkPolarisTrackerTest.cxx
     igstkAtamaiNDITrackerTest.cxx
   ) 

#copy them to bin directory
FOREACH(SourceFile ${IGSTK_TEST_DEPEND_ON_DEVICES_SRCS})
  IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Testing/${SourceFile})
    SET(TestFullPath ${IGSTKSandbox_SOURCE_DIR}/Testing/${SourceFile})
  ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Testing/${SourceFile})
    SET(TestFullPath ${IGSTK_SOURCE_DIR}/Testing/${SourceFile})
  ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/Testing/${SourceFile})

  CONFIGURE_FILE( ${TestFullPath} ${IGSTK_TEST_FILES_COMBINED} COPYONLY )
ENDFOREACH( SourceFile )


IF(IGSTK_TEST_AURORA_ATTACHED)
  ADD_TEST(igstkAuroraTrackerTest ${IGSTKSandbox_TESTS} igstkAuroraTrackerTest)
  SET(BasicTests_SRCS ${BasicTests_SRCS} 
              ${IGSTK_TEST_FILES_COMBINED}/igstkAuroraTrackerTest.cxx)
ENDIF(IGSTK_TEST_AURORA_ATTACHED)

IF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)
  ADD_TEST(igstkNDICommandInterpreterTest ${IGSTKSandbox_TESTS} igstkNDICommandInterpreterTest)
  SET(BasicTests_SRCS ${BasicTests_SRCS} 
              ${IGSTK_TEST_FILES_COMBINED}/igstkNDICommandInterpreterTest.cxx)
ENDIF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)

IF(IGSTK_TEST_POLARIS_ATTACHED)  
  ADD_TEST(igstkPolarisTrackerTest ${IGSTKSandbox_TESTS} igstkPolarisTrackerTest)
  SET(BasicTests_SRCS ${BasicTests_SRCS} 
              ${IGSTK_TEST_FILES_COMBINED}/igstkPolarisTrackerTest.cxx)
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

IF (IGSTK_USE_ATAMAI)
  ADD_TEST(igstkAtamaiNDITrackerTest ${IGSTKSandbox_TESTS} igstkAtamaiNDITrackerTest)
  SET(BasicTests_SRCS ${BasicTests_SRCS} 
              ${IGSTK_TEST_FILES_COMBINED}/igstkAtamaiNDITrackerTest.cxx)
ENDIF (IGSTK_USE_ATAMAI)

#Copy also the test driver to TEST_COMBINED directory
CONFIGURE_FILE(${IGSTKSandbox_SOURCE_DIR}/Testing/igstkSandboxTests.cxx 
               ${IGSTK_TEST_FILES_COMBINED} COPYONLY )
ADD_EXECUTABLE(igstkSandboxTests 
               ${IGSTK_TEST_FILES_COMBINED}/igstkSandboxTests.cxx 
               ${BasicTests_SRCS})
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

