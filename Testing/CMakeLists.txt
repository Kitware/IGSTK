# make a directory to copy latest versions of all source and header files
SET(IGSTK_TESTS_LATEST  ${IGSTKSandbox_BINARY_DIR}/TestsLatest )
MAKE_DIRECTORY( ${IGSTK_TESTS_LATEST} )

SET(IGSTKSandbox_TESTS ${CXX_TEST_PATH}/igstkSandboxTests)

SET(IGSTKSandbox_TEST_OUTPUT_DIR "${IGSTKSandbox_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTKSandbox_TEST_OUTPUT_DIR})

SET(IGSTKSandbox_DICOM_TEST_OUTPUT_DIR "${IGSTKSandbox_TEST_OUTPUT_DIR}/DICOM")
MAKE_DIRECTORY(${IGSTKSandbox_DICOM_TEST_OUTPUT_DIR})

SET(IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR "${IGSTKSandbox_TEST_OUTPUT_DIR}/StateMachineDiagrams")
MAKE_DIRECTORY(${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})

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

INCLUDE_DIRECTORIES (
  ${IGSTK_SOURCE_DIR}/Testing
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source
  ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/Source
  ${IGSTKSandbox_SOURCE_DIR}/Testing
  ${IGSTKSandbox_BINARY_DIR}/Testing
  ${IGSTKSandbox_SOURCE_DIR}/SourceLatest
  ${IGSTKSandbox_SOURCE_DIR}/TestingLatest
  )

# Add your new tests here
SET(SandboxTests_SRCS
  igstkPivotCalibrationAlgorithmTest.cxx
  igstkDICOMGenericImageReaderTest.cxx
  igstkGenericImageSpatialObjectTest.cxx
  igstkLandmarkUltrasoundCalibrationTest.cxx
  igstkSocketCommunicationTest.cxx
  igstkViewNewTest.cxx
  )



ADD_TEST( igstkDICOMGenericImageReaderTest ${IGSTKSandbox_TESTS} igstkDICOMGenericImageReaderTest
   ${IGSTK_DATA_ROOT}/Input/E000192 )

ADD_TEST( igstkGenericImageSpatialObjectTest ${IGSTKSandbox_TESTS} igstkGenericImageSpatialObjectTest)

ADD_TEST( igstkLandmarkUltrasoundCalibrationTest ${IGSTKSandbox_TESTS} igstkLandmarkUltrasoundCalibrationTest)
ADD_TEST( igstkSocketCommunicationTest ${IGSTKSandbox_TESTS} igstkSocketCommunicationTest)
ADD_TEST( igstkViewNewTest ${IGSTKSandbox_TESTS} igstkViewNewTest)
ADD_TEST( igstkPivotCalibrationAlgorithmTest ${IGSTKSandbox_TESTS} igstkPivotCalibrationAlgorithmTest)

IF(IGSTKSandbox_USE_FLTK)
    ADD_TEST( igstkFLTKWidgetTest
              ${IGSTKSandbox_TESTS}
              igstkFLTKWidgetTest)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkFLTKWidgetTest.cxx)
ENDIF(IGSTKSandbox_USE_FLTK)

IF(IGSTKSandbox_USE_Qt)
    ADD_TEST( igstkQTWidgetTest
              ${IGSTKSandbox_TESTS}
              igstkQTWidgetTest)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkQTWidgetTest.cxx)
ENDIF(IGSTKSandbox_USE_Qt)


IF(IGSTKSandbox_DATA_ROOT)
  SET(IGSTKSandbox_BASELINE_DIR   "${IGSTKSandbox_DATA_ROOT}/Baseline")
  IF (IGSTK_USE_FLTK)
    ADD_TEST( igstkObliqueImageSpatialObjectRepresentationTest
              ${IGSTKSandbox_TESTS}
              igstkObliqueImageSpatialObjectRepresentationTest
              ${IGSTKSandbox_DATA_ROOT}/E000192)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkObliqueImageSpatialObjectRepresentationTest)
  ENDIF (IGSTK_USE_FLTK)
ENDIF(IGSTKSandbox_DATA_ROOT)

ADD_EXECUTABLE(igstkSandboxTests igstkSandboxTests.cxx ${SandboxTests_SRCS})

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


#-----------------------------------------------------------------------------
# Now running IGSTK tests with the latest main library
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkLatestTests)

# You just need to add your new tests here
SET(IGSTKTests_SRCS
  igstkFlockOfBirdsCommandInterpreterTest.cxx
  igstkFlockOfBirdsCommandInterpreterFBBTest.cxx
  igstkFlockOfBirdsTrackerTest.cxx
  igstkVesselObjectTest.cxx
  igstkUSImageObjectTest.cxx
  igstkUSImageObjectRepresentationTest.cxx
  igstkToolCalibrationTest.cxx
  igstkBasicTrackerTest.cxx
  igstkBinaryDataTest.cxx
  igstkCommunicationTest.cxx
  igstkCTImageSpatialObjectTest.cxx
  igstkImageReaderTest.cxx
  igstkImageSpatialObjectTest.cxx
  igstkLandmark3DRegistrationTest.cxx
  igstkLandmark3DRegistrationTest2.cxx
  igstkLandmark3DRegistrationErrorEstimatorTest.cxx
  igstkMRImageSpatialObjectRepresentationTest.cxx
  igstkMRImageSpatialObjectTest.cxx
  igstkMultipleOutputTest.cxx
  igstkPivotCalibrationTest.cxx
  igstkPrincipalAxisCalibrationTest.cxx
  igstkSerialCommunicationTest.cxx
  igstkSpatialObjectTest.cxx
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

IF (IGSTK_USE_FLTK)

  SET(IGSTKTests_SRCS ${IGSTKTests_SRCS}
    igstkAxesObjectTest.cxx
    igstkBoxObjectTest.cxx
    igstkConeObjectTest.cxx
    igstkMouseTrackerTest.cxx
    igstkUltrasoundImageSimulatorTest.cxx
    igstkUltrasoundProbeObjectTest.cxx
  )

ENDIF (IGSTK_USE_FLTK)


# Add testing command
FOREACH(TestFiles ${IGSTKTests_SRCS})
  GET_FILENAME_COMPONENT(TestProgram ${TestFiles} NAME_WE)
  ADD_TEST(${TestProgram} ${IGSTK_TESTS} ${TestProgram})
ENDFOREACH(TestFiles)

#-----------------------------------------------------------------------------
# Tests need data input
IF (IGSTK_DATA_ROOT)

  ADD_TEST( igstkUSImageReaderTest ${IGSTK_TESTS} igstkUSImageReaderTest
            ${IGSTK_DATA_ROOT}/Input/USLiver )
  ADD_TEST( igstkToolCalibrationReaderTest ${IGSTK_TESTS}
             igstkToolCalibrationReaderTest
             ${IGSTK_DATA_ROOT}/Input/PivotCalibrationTool.txt
             ${IGSTK_DATA_ROOT}/Input/PivotCalibrationToolCorruptedOnPupose.txt
             )
  ADD_TEST( igstkPivotCalibrationReaderTest ${IGSTK_TESTS}
             igstkPivotCalibrationReaderTest
             ${IGSTK_DATA_ROOT}/Input/PivotCalibrationTool.txt
             ${IGSTK_DATA_ROOT}/Input/PivotCalibrationToolCorruptedOnPupose.txt
             )
  ADD_TEST(igstkMR3DImageToUS3DImageRegistrationTest ${IGSTK_TESTS}
           igstkMR3DImageToUS3DImageRegistrationTest
           ${IGSTK_DATA_ROOT}/Input/MRLiver)

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
       ${IGSTK_DATA_ROOT}/Input/MRLiver )
  ADD_TEST( igstkMeshReaderTest ${IGSTK_TESTS} igstkMeshReaderTest
       ${IGSTK_DATA_ROOT}/Input/liver.msh
       ${IGSTK_DATA_ROOT}/Input/liverCorruptedOnPurpose.msh )
 ADD_TEST( igstkTubeVesselReaderTest ${IGSTK_TESTS} igstkTubeReaderTest
       ${IGSTK_DATA_ROOT}/Input/vessel.tre
       ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre
       ${IGSTK_DATA_ROOT}/Input/vesselWithoutReadPermissions.tre )
  ADD_TEST( igstkTubeReaderTest ${IGSTK_TESTS} igstkTubeReaderTest
       ${IGSTK_DATA_ROOT}/Input/Tube.tre
       ${IGSTK_DATA_ROOT}/Input/TubeCorruptedOnPurpose.tre
       ${IGSTK_DATA_ROOT}/Input/TubeWithoutReadPermissions.tre )
  ADD_TEST( igstkSpatialObjectReaderTest ${IGSTK_TESTS} igstkSpatialObjectReaderTest
       ${IGSTK_DATA_ROOT}/Input/vessel.tre
       ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre
       ${IGSTK_DATA_ROOT}/Input/vesselWithoutReadPermissions.tre )
  ADD_TEST( igstkCTImageSpatialObjectRepresentationTest ${IGSTK_TESTS}
       igstkCTImageSpatialObjectRepresentationTest
       ${IGSTK_DATA_ROOT}/Input/E000192)
  ADD_TEST( igstkDICOMImageReaderErrorsTest ${IGSTK_TESTS} igstkDICOMImageReaderErrorsTest
       ${IGSTK_DATA_ROOT}/Input/E000192Mod ${IGSTKSandbox_DICOM_TEST_OUTPUT_DIR})

  IF (IGSTK_USE_FLTK)
    ADD_TEST( igstkAnnotation2DTest ${IGSTK_TESTS} igstkAnnotation2DTest
              ${IGSTK_DATA_ROOT}/Input/E000192 )
  ENDIF (IGSTK_USE_FLTK)

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
  ADD_TEST(igstkMeshObjectTest ${IGSTK_TESTS} igstkMeshObjectTest)
  ADD_TEST(igstkPulseGeneratorTest ${IGSTK_TESTS} igstkPulseGeneratorTest)
  ADD_TEST(igstkTubeObjectTest ${IGSTK_TESTS} igstkTubeObjectTest)
  ADD_TEST(igstkViewTest ${IGSTK_TESTS} igstkViewTest)
  ADD_TEST(igstkViewRefreshRateTest ${IGSTK_TESTS} igstkViewRefreshRateTest)
  ADD_TEST(igstkSpatialObjectRepresentationVisibilityTest ${IGSTK_TESTS} igstkSpatialObjectRepresentationVisibilityTest)

  IF (IGSTK_DATA_ROOT)

     ADD_TEST( igstkImageSpatialObjectRepresentationTest2
       ${IGSTK_TESTS}
       --compare ${IGSTKSandbox_BASELINE_DIR}/igstkImageSpatialObjectRepresentationTest2.png
       ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest2.png
       --toleranceIntensity 10
       --toleranceRadius 5
       --toleranceNumberOfPixels 5
       igstkImageSpatialObjectRepresentationTest2
       ${IGSTKSandbox_DATA_ROOT}/E000192
       ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest2.png)

    ADD_TEST( igstkImageSpatialObjectRepresentationTest3
       ${IGSTK_TESTS}
       --compare ${IGSTKSandbox_BASELINE_DIR}/igstkImageSpatialObjectRepresentationTest3.png
       ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest3.png
       --toleranceIntensity 10
       --toleranceRadius 5
       --toleranceNumberOfPixels 5
       igstkImageSpatialObjectRepresentationTest3
       ${IGSTKSandbox_DATA_ROOT}/E000192Mod2
       ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest3.png)

    ADD_TEST( igstkVascularNetworkReaderTest ${IGSTK_TESTS}
          --compare ${IGSTKSandbox_BASELINE_DIR}/igstkVascularNetworkReaderTest.png
          ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkVascularNetworkReaderTest.png
          --toleranceIntensity 10
          --toleranceRadius 5
          --toleranceNumberOfPixels 5
          igstkVascularNetworkReaderTest
          ${IGSTK_DATA_ROOT}/Input/vessel.tre
          ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre
          ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkVascularNetworkReaderTest.png
          )

    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              --toleranceIntensity 10
              --toleranceRadius 5
              --toleranceNumberOfPixels 5
              igstkCTImageSpatialObjectReadingAndRepresentationTest
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )

    ADD_TEST( igstkImageSpatialObjectRepresentationTest ${IGSTK_TESTS}
              igstkImageSpatialObjectRepresentationTest
              ${IGSTK_DATA_ROOT}/Input/E000192)

    ADD_TEST( igstkUltrasoundImageSimulatorTest ${IGSTK_TESTS}
              --compare ${IGSTKSandbox_BASELINE_DIR}/igstkUltrasoundImageSimulatorTest.png
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkUltrasoundImageSimulatorTest.png
              --toleranceIntensity 10
              --toleranceRadius 5
              --toleranceNumberOfPixels 5
              igstkUltrasoundImageSimulatorTest
              ${IGSTK_DATA_ROOT}/Input/MRLiver
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkUltrasoundImageSimulatorTest.png)

  ENDIF (IGSTK_DATA_ROOT)

ENDIF(IGSTK_USE_FLTK)

#-----------------------------------------------------------------------------
# Testing source file depend on external device
#IF(IGSTK_TEST_AURORA_ATTACHED)
  SET(IGSTKTests_SRCS ${IGSTKTests_SRCS}
    igstkAuroraTrackerTest.cxx
  )
#ENDIF(IGSTK_TEST_AURORA_ATTACHED)

#IF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)
  SET(IGSTKTests_SRCS ${IGSTKTests_SRCS}
    igstkNDICommandInterpreterTest.cxx
  )
#ENDIF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)

#IF(IGSTK_TEST_POLARIS_ATTACHED)
  SET(IGSTKTests_SRCS ${IGSTKTests_SRCS}
    igstkPolarisTrackerTest.cxx
  )
#ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

#-----------------------------------------------------------------------------
# Testing source file need data input
IF(IGSTK_DATA_ROOT)
  SET(IGSTKTests_SRCS
    ${IGSTKTests_SRCS}
    igstkUSImageReaderTest.cxx
    igstkPivotCalibrationReaderTest.cxx
    igstkToolCalibrationReaderTest.cxx
    igstkMR3DImageToUS3DImageRegistrationTest.cxx
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
  SET(IGSTKTests_SRCS
    ${IGSTKTests_SRCS}
    igstkAnnotation2DTest.cxx
    igstkCylinderObjectTest.cxx
    igstkEllipsoidObjectTest.cxx
    igstkFLTKTextBufferLogOutputTest.cxx
    igstkFLTKTextLogOutputTest.cxx
    igstkMeshObjectTest.cxx
    igstkPulseGeneratorTest.cxx
    igstkTubeObjectTest.cxx
    igstkViewTest.cxx
    igstkViewRefreshRateTest.cxx
    igstkSpatialObjectRepresentationVisibilityTest.cxx
  )
  IF(IGSTK_DATA_ROOT)
    SET(IGSTKTests_SRCS
      ${IGSTKTests_SRCS}
      igstkImageSpatialObjectRepresentationTest2.cxx
      igstkImageSpatialObjectRepresentationTest3.cxx
      igstkVascularNetworkReaderTest.cxx
      igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx
      igstkImageSpatialObjectRepresentationTest.cxx
    )
  ENDIF(IGSTK_DATA_ROOT)
ENDIF(IGSTK_USE_FLTK)

#-----------------------------------------------------------------------------
# Copy the source files if they are not a duplicated in the Sandbox
FOREACH(SourceFile ${IGSTKTests_SRCS})

  IF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing/${SourceFile})
     SET(IGSTKLatestTests_SRCS ${IGSTKLatestTests_SRCS} ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing/${SourceFile})
     EXEC_PROGRAM("@CMAKE_COMMAND@" ARGS "-E remove \"${IGSTK_TESTS_LATEST}/${SourceFile}\""
                   OUTPUT_VARIABLE rm_out
                   RETURN_VARIABLE rm_retval )
     SET( TestsModifiedFiles ${TestsModifiedFiles} ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing/${SourceFile} )
  ELSE(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing/${SourceFile})
    # Copy the files from IGSTK
    CONFIGURE_FILE( ${IGSTK_SOURCE_DIR}/Testing/${SourceFile} ${IGSTK_TESTS_LATEST}/${SourceFile} COPYONLY)
    SET(IGSTKLatestTests_SRCS ${IGSTKLatestTests_SRCS} ${IGSTK_TESTS_LATEST}/${SourceFile})
  ENDIF(EXISTS ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing/${SourceFile})

ENDFOREACH( SourceFile )

#-----------------------------------------------------------------------------
# Copy the .NoDartCoverage file to the duplicate testing directory
CONFIGURE_FILE( ${IGSTK_SOURCE_DIR}/Testing/.NoDartCoverage ${IGSTK_TESTS_LATEST}/.NoDartCoverage COPYONLY)

SOURCE_GROUP(TestsModified FILES ${TestsModifiedFiles})

#-----------------------------------------------------------------------------
# Add testing executables
ADD_EXECUTABLE(igstkLatestTests ${IGSTK_SOURCE_DIR}/Testing/igstkTests.cxx ${IGSTKLatestTests_SRCS})
TARGET_LINK_LIBRARIES(igstkLatestTests IGSTKSandbox)

#ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
#ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
#TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)

#ADD_EXECUTABLE(igstkStateMachineExportTest igstkStateMachineExportTest.cxx)
#ADD_TEST(igstkStateMachineExportTest ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
#TARGET_LINK_LIBRARIES(igstkStateMachineExportTest IGSTK)
