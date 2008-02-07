# make a directory to copy latest versions of all source and header files
SET(IGSTK_TESTS_LATEST  ${IGSTKSandbox_BINARY_DIR}/TestsLatest )
MAKE_DIRECTORY( ${IGSTK_TESTS_LATEST} )

SET(IGSTKSandbox_TESTS ${CXX_TEST_PATH}/igstkSandboxTests)
SET(IGSTKSandboxTrackerSuppervised_TESTS ${CXX_TEST_PATH}/igstkTrackerSupervisedTests)

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
  igstkSpatialObjectCoordinateSystemTest.cxx
  igstkCoordinateReferenceSystemTest.cxx
  igstkCoordinateReferenceSystemTest2.cxx
  igstkCoordinateReferenceSystemTest3.cxx
  igstkCoordinateReferenceSystemDelegatorTest.cxx
  igstkSpatialObjectCoordinateSystemTest3.cxx
  igstkGroupObjectTest.cxx
  igstkPivotCalibrationAlgorithmTest.cxx
  igstkDICOMGenericImageReaderTest.cxx
  igstkGenericImageSpatialObjectTest.cxx
  igstkLandmarkUltrasoundCalibrationTest.cxx
  igstkSocketCommunicationTest.cxx
  igstkVascularNetworkObjectTest.cxx
  igstkTransductionMacroTest.cxx
  igstkObjectRepresentationRemovalTest.cxx
  )

IF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)
  SET(SandboxTests_SRCS
    ${SandboxTests_SRCS}
    igstkTrackerToolObserverToSocketRelayTest.cxx
    igstkPolarisTrackerToolObserverToSocketRelayTest.cxx
    igstkVicraTrackerToolObserverToSocketRelayTest.cxx
    igstkTransformSocketListenerTest.cxx
    )
ENDIF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)

IF(IGSTK_TEST_POLARIS_ATTACHED)
  SET(SandboxTests_SRCS ${SandboxTests_SRCS}
    igstkPolarisTrackerTest2.cxx )

  SET(SandboxTests_SRCS ${SandboxTests_SRCS}
    igstkPolarisTrackerTest3.cxx )

  SET(SandboxTests_SRCS ${SandboxTests_SRCS}
    igstkPolarisTrackerTest4.cxx )

  SET(SandboxTests_SRCS ${SandboxTests_SRCS}
    igstkPolarisTrackerToolTest.cxx
  )
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

IF(IGSTK_TEST_AURORA_ATTACHED)
  SET(SandboxTests_SRCS ${SandboxTests_SRCS}
    igstkAuroraTrackerToolTest.cxx
  )
ENDIF(IGSTK_TEST_AURORA_ATTACHED)


ADD_TEST( igstkDICOMGenericImageReaderTest ${IGSTKSandbox_TESTS} igstkDICOMGenericImageReaderTest
   ${IGSTK_DATA_ROOT}/Input/E000192 )

ADD_TEST( igstkGenericImageSpatialObjectTest ${IGSTKSandbox_TESTS} igstkGenericImageSpatialObjectTest)

ADD_TEST( igstkLandmarkUltrasoundCalibrationTest ${IGSTKSandbox_TESTS} igstkLandmarkUltrasoundCalibrationTest
  ${IGSTKSandbox_DATA_ROOT}/Input
  )

ADD_TEST( igstkSocketCommunicationTest ${IGSTKSandbox_TESTS} igstkSocketCommunicationTest)
ADD_TEST( igstkTransductionMacroTest ${IGSTKSandbox_TESTS} igstkTransductionMacroTest)

IF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)
  ADD_TEST( igstkTransformSocketListenerTest 
    ${IGSTKSandbox_TESTS} 
    igstkTransformSocketListenerTest
    16666 1000
    )

  ADD_TEST( igstkTrackerToolObserverToSocketRelayTest 
    ${IGSTKSandbox_TESTS} 
    igstkTrackerToolObserverToSocketRelayTest
    localhost 16666 1000 30 10
    )

  ADD_TEST( igstkPolarisTrackerToolObserverToSocketRelayTest 
    ${IGSTKSandbox_TESTS} 
    igstkPolarisTrackerToolObserverToSocketRelayTest
    localhost 16666 1000
    )

  ADD_TEST( igstkVicraTrackerToolObserverToSocketRelayTest 
    ${IGSTKSandbox_TESTS} 
    igstkPolarisTrackerToolObserverToSocketRelayTest
    localhost 16666 1000
    )

ENDIF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)

ADD_TEST( igstkObjectRepresentationRemovalTest ${IGSTKSandbox_TESTS} igstkObjectRepresentationRemovalTest)
ADD_TEST( igstkPivotCalibrationAlgorithmTest ${IGSTKSandbox_TESTS} igstkPivotCalibrationAlgorithmTest)
ADD_TEST( igstkCoordinateReferenceSystemTest ${IGSTKSandbox_TESTS} igstkCoordinateReferenceSystemTest)
ADD_TEST( igstkCoordinateReferenceSystemTest2 ${IGSTKSandbox_TESTS} igstkCoordinateReferenceSystemTest2)
ADD_TEST( igstkCoordinateReferenceSystemTest3 ${IGSTKSandbox_TESTS} igstkCoordinateReferenceSystemTest3)
ADD_TEST( igstkCoordinateReferenceSystemDelegatorTest ${IGSTKSandbox_TESTS} igstkCoordinateReferenceSystemDelegatorTest)
ADD_TEST( igstkSpatialObjectCoordinateSystemTest3 ${IGSTKSandbox_TESTS} igstkSpatialObjectCoordinateSystemTest3)
ADD_TEST( igstkGroupObjectTest ${IGSTKSandbox_TESTS} igstkGroupObjectTest)
ADD_TEST( igstkVascularNetworkObjectTest ${IGSTKSandbox_TESTS} igstkVascularNetworkObjectTest)

IF(IGSTK_TEST_POLARIS_ATTACHED)
    ADD_TEST( igstkPolarisTrackerTest2  
              ${IGSTKSandbox_TESTS}
              igstkPolarisTrackerTest2
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest2LoggerOutput.txt
              ${IGSTKSandbox_DATA_ROOT}/Input/Passive_4Marker_Planar_Rigid_Body_8700302.rom
              )
    ADD_TEST( igstkPolarisTrackerTest3  
              ${IGSTKSandbox_TESTS}
              igstkPolarisTrackerTest3
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest3LoggerOutput.txt
              ${IGSTK_DATA_ROOT}/ta2p0003-3-120.rom
              )
    ADD_TEST( igstkPolarisTrackerTest4  
              ${IGSTKSandbox_TESTS}
              igstkPolarisTrackerTest4
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest4LoggerOutput.txt
              0
              1
              )
 
  ADD_TEST(igstkPolarisTrackerToolTest ${IGSTKSandbox_TESTS}
igstkPolarisTrackerToolTest)
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

IF(IGSTK_TEST_AURORA_ATTACHED)
 ADD_TEST(igstkAuroraTrackerToolTest ${IGSTKSandbox_TESTS}
igstkAuroraTrackerToolTest)
ENDIF(IGSTK_TEST_AURORA_ATTACHED)

IF(IGSTKSandbox_USE_FLTK)

# FIXME: When done debugging, register it inside of the test drivers.
ADD_EXECUTABLE( igstkCTImageSpatialObjectReadingAndRepresentationTest4 igstkCTImageSpatialObjectReadingAndRepresentationTest4.cxx)
TARGET_LINK_LIBRARIES( igstkCTImageSpatialObjectReadingAndRepresentationTest4 IGSTKSandbox )
ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest4  
  ${CXX_TEST_PATH}/igstkCTImageSpatialObjectReadingAndRepresentationTest4
  ${IGSTK_DATA_ROOT}/Input/E000192 )
# END OF FIXME

    ADD_TEST( igstkMouseTrackerToolTest ${IGSTKSandbox_TESTS} igstkMouseTrackerToolTest)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS} igstkMouseTrackerToolTest.cxx)

    ADD_TEST( igstkFLTKWidgetTest ${IGSTKSandbox_TESTS} igstkFLTKWidgetTest)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS} igstkFLTKWidgetTest.cxx)

    # A first test to compare the one screenshot from a FLTKWidget
    ADD_TEST( igstkFLTKWidgetTest2_1
              ${IGSTKSandbox_TESTS}
              --compare ${IGSTKSandbox_DATA_ROOT}/Baseline/igstkFLTKWidgetTest2Screenshot1.png 
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot1.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 10 
              igstkFLTKWidgetTest2
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot1.png 
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot2.png 
              )

    # A second test to compare the second screenshot
    ADD_TEST( igstkFLTKWidgetTest2_2
              ${IGSTKSandbox_TESTS}
              --compare ${IGSTKSandbox_DATA_ROOT}/Baseline/igstkFLTKWidgetTest2Screenshot2.png 
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot2.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 10 
              igstkFLTKWidgetTest2
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot1.png 
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot2.png 
              )

    SET(SandboxTests_SRCS ${SandboxTests_SRCS} igstkFLTKWidgetTest2.cxx)

    ADD_TEST( igstkCoordinateReferenceSystemObjectWithViewTest
              ${IGSTKSandbox_TESTS}
              --compare ${IGSTKSandbox_DATA_ROOT}/Baseline/igstkCoordinateSystemTestScreenshot1.png 
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkCoordinateSystemTestScreenshot1.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkCoordinateReferenceSystemObjectWithViewTest
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkCoordinateSystemTestScreenshot1.png 
              )

    SET(SandboxTests_SRCS ${SandboxTests_SRCS} 
              igstkCoordinateReferenceSystemObjectWithViewTest.cxx)

    ADD_TEST( igstkCircularSimulatedTrackerTest
              ${IGSTKSandbox_TESTS}
              igstkCircularSimulatedTrackerTest
              )

    SET(SandboxTests_SRCS ${SandboxTests_SRCS} 
              igstkCircularSimulatedTrackerTest.cxx)

ENDIF(IGSTKSandbox_USE_FLTK)

IF(IGSTKSandbox_USE_Qt)
    ADD_TEST( igstkQTWidgetTest
              ${IGSTKSandbox_TESTS}
              igstkQTWidgetTest)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkQTWidgetTest.cxx)

    ADD_TEST( igstkQTWidgetTest2
              ${IGSTKSandbox_TESTS}
              igstkQTWidgetTest2)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkQTWidgetTest2.cxx)
 
    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest3 
              ${IGSTKSandbox_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png 
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkViewScreenShot.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkCTImageSpatialObjectReadingAndRepresentationTest3
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkCTImageSpatialObjectReadingAndRepresentationTest3.cxx)
ENDIF(IGSTKSandbox_USE_Qt)

IF(IGSTKSandbox_USE_MicronTracker)
    ADD_TEST( igstkMicronTrackerTest
              ${IGSTKSandbox_TESTS}
              igstkMicronTrackerTest
              ${IGSTKSandbox_DATA_ROOT}/Input/CalibrationFiles
              ${IGSTKSandbox_DATA_ROOT}/Input/MicronTracker.ini
              ${IGSTKSandbox_DATA_ROOT}/Input/Markers
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkMicronTrackerTestLogOutput.txt)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkMicronTrackerTest.cxx)

    ADD_TEST( igstkMicronTrackerToolTest
              ${IGSTKSandbox_TESTS} igstkMicronTrackerToolTest ) 
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkMicronTrackerToolTest.cxx)

ENDIF(IGSTKSandbox_USE_MicronTracker)


IF(IGSTKSandbox_DATA_ROOT)
  SET(IGSTKSandbox_BASELINE_DIR   "${IGSTKSandbox_DATA_ROOT}/Baseline")
  IF (IGSTKSandbox_USE_FLTK)
    ADD_TEST( igstkObliqueImageSpatialObjectRepresentationTest
              ${IGSTKSandbox_TESTS}
              igstkObliqueImageSpatialObjectRepresentationTest
              ${IGSTKSandbox_DATA_ROOT}/E000192)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkObliqueImageSpatialObjectRepresentationTest.cxx)

    ADD_TEST( igstkAnnotation2DTest2 ${IGSTKSandbox_TESTS} 
       --compare ${IGSTKSandbox_BASELINE_DIR}/igstkAnnotation2DTest2ScreenShot.png
       ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAnnotation2DTest2ScreenShot.png
              igstkAnnotation2DTest2
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAnnotation2DTest2ScreenShot.png)
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkAnnotation2DTest2.cxx)

    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest2 
              ${IGSTKSandbox_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png 
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkViewScreenShot.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkCTImageSpatialObjectReadingAndRepresentationTest2
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )

    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkCTImageSpatialObjectReadingAndRepresentationTest2.cxx)

  ENDIF (IGSTKSandbox_USE_FLTK)
ENDIF(IGSTKSandbox_DATA_ROOT)

IF( IGSTKSandbox_BUILD_SUPERVISED_TRACKER_TESTING )

  # Add your supervised tracker new tests here
  SET(TrackerSupervisedTests_SRCS
    igstkPolarisHardwareTest.cxx
    )

  ADD_TEST( igstkPolarisHardwareTest ${IGSTKSandboxTrackerSuppervised_TESTS} igstkPolarisHardwareTest
    ${IGSTK_DATA_ROOT}/Baseline/PolarisHardwarePositionsAndOrientations.txt
    ${IGSTKSandbox_TEST_OUTPUT_DIR}/PolarisHardwarePositionsAndOrientationsLogOutput.txt )

  ADD_EXECUTABLE(igstkTrackerSupervisedTests igstkTrackerSupervisedTests.cxx ${TrackerSupervisedTests_SRCS})
  TARGET_LINK_LIBRARIES(igstkTrackerSupervisedTests IGSTKSandbox)

ENDIF( IGSTKSandbox_BUILD_SUPERVISED_TRACKER_TESTING )

ADD_EXECUTABLE(igstkSandboxTests igstkSandboxTests.cxx ${SandboxTests_SRCS})
TARGET_LINK_LIBRARIES(igstkSandboxTests IGSTKSandbox)

ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)


ADD_EXECUTABLE(igstkStateMachineExportTest2 igstkStateMachineExportTest2.cxx)
ADD_TEST(igstkStateMachineExportTest2 ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest2 ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
TARGET_LINK_LIBRARIES(igstkStateMachineExportTest2 IGSTKSandbox)

# Configure a header needed by SystemInformation.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/igstkSystemInformation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/igstkSystemInformation.h"
               @ONLY IMMEDIATE)


#-----------------------------------------------------------------------------
# Now running IGSTK tests with the latest main library
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkLatestTests)

# You just need to add your new tests here
SET(IGSTKTests_SRCS
# FIXME: comment out the FlockOfBirds tests just for the
# time being. It requires major work to make it consistent 
# with the new Tracker API
#
#  igstkFlockOfBirdsCommandInterpreterTest.cxx
#  igstkFlockOfBirdsCommandInterpreterFBBTest.cxx
#  igstkFlockOfBirdsTrackerTest.cxx
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
  igstkSpatialObjectRepresentationVisibilityTest.cxx
  igstkStateMachineErrorsTest.cxx
  igstkStateMachineTest.cxx
  igstkStringEventTest.cxx
  igstkTimeStampTest.cxx
  igstkTokenTest.cxx
  igstkTrackerTest.cxx
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

# Add generic test that require input data
ADD_TEST( igstkSpatialObjectRepresentationVisibilityTest 
            ${IGSTK_TESTS}
            --compare ${IGSTKSandbox_BASELINE_DIR}/igstkSpatialObjectRepresentationVisibilityTest.png 
                      ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest.png 
            --toleranceIntensity 10
            --toleranceRadius    5
            --toleranceNumberOfPixels 5 
           igstkSpatialObjectRepresentationVisibilityTest
            ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest.png
            ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest2.png
        )


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

 
  ADD_TEST( igstkAuroraTrackerSimulatedTest  
              ${IGSTK_TESTS}
              igstkAuroraTrackerSimulatedTest
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAuroraTrackerSimulatedTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/Input/aurora_stream_multichan_11_11_2005.txt
              ${IGSTK_DATA_ROOT}/Input/aurora_multichan.rom              )

  ADD_TEST(igstkNDICommandInterpreterSimulatedTest ${IGSTK_TESTS} igstkNDICommandInterpreterSimulatedTest)
  ADD_TEST(igstkNDICommandInterpreterStressTest ${IGSTK_TESTS} igstkNDICommandInterpreterStressTest)

  ADD_TEST( igstkPolarisTrackerSimulatedTest  
              ${IGSTK_TESTS}
              igstkPolarisTrackerSimulatedTest
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkPolarisTrackerSimulatedTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/polaris_stream_11_05_2005.txt
              ${IGSTK_DATA_ROOT}/ta2p0003-3-120.rom
              )

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
    ADD_TEST( igstkAnnotation2DTest1 ${IGSTK_TESTS} 
       --compare ${IGSTKSandbox_BASELINE_DIR}/igstkAnnotation2DTest1ScreenShot.png
       ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAnnotation2DTest1ScreenShot.png
              igstkAnnotation2DTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAnnotation2DTest1ScreenShot.png)
  ENDIF (IGSTK_USE_FLTK)

ENDIF(IGSTK_DATA_ROOT)

#-----------------------------------------------------------------------------
# Tests depend on external device
IF(IGSTK_TEST_AURORA_ATTACHED)
  ADD_TEST(igstkAuroraTrackerTest 
           ${IGSTK_TESTS}
           igstkAuroraTrackerTest
           ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAuroraTrackerTestLoggerOutput.txt
           ${IGSTK_TEST_AURORA_PORT_NUMBER}
           )
  ADD_TEST(igstkAuroraTrackerTest2 
           ${IGSTK_TESTS}
           igstkAuroraTrackerTest2
           ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAuroraTrackerTest2LoggerOutput.txt
           ${IGSTK_TEST_AURORA_PORT_NUMBER}
           )
ENDIF(IGSTK_TEST_AURORA_ATTACHED)

IF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)
  ADD_TEST(igstkNDICommandInterpreterTest ${IGSTK_TESTS} igstkNDICommandInterpreterTest)
ENDIF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)

IF(IGSTK_TEST_POLARIS_ATTACHED)
    ADD_TEST( igstkPolarisTrackerTest  
              ${IGSTK_TESTS}
              igstkPolarisTrackerTest
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkPolarisTrackerTestLoggerOutput.txt
              ${IGSTKSandbox_DATA_ROOT}/Input/Passive_4Marker_Planar_Rigid_Body_8700302.rom
              )
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

    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest1
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

    ADD_TEST( igstkCTImageSpatialObjectRepresentationWindowLevelTest 
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkWindowLevelScreenShot01.png 
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkWindowLevelScreenShot01.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkCTImageSpatialObjectRepresentationWindowLevelTest
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkWindowLevelScreenShot01.png
              )

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
  SET(IGSTKTests_SRCS ${IGSTKTests_SRCS}
    igstkAuroraTrackerTest2.cxx
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
  )
  IF(IGSTK_DATA_ROOT)
    SET(IGSTKTests_SRCS
      ${IGSTKTests_SRCS}
      igstkImageSpatialObjectRepresentationTest2.cxx
      igstkImageSpatialObjectRepresentationTest3.cxx
      igstkVascularNetworkReaderTest.cxx
      igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx
      igstkCTImageSpatialObjectRepresentationWindowLevelTest.cxx
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
ADD_EXECUTABLE(igstkLatestTests ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing/igstkTests.cxx ${IGSTKLatestTests_SRCS})
TARGET_LINK_LIBRARIES(igstkLatestTests IGSTKSandbox)

#ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
#ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
#TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)

ADD_EXECUTABLE(igstkStateMachineExportTest ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing/igstkStateMachineExportTest.cxx)
ADD_TEST(igstkStateMachineExportTest ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
TARGET_LINK_LIBRARIES(igstkStateMachineExportTest IGSTKSandbox)

