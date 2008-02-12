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
ADD_TEST(igstkVesselObjectTest ${IGSTK_TESTS} igstkVesselObjectTest)
ADD_TEST(igstkUSImageObjectTest ${IGSTK_TESTS} igstkUSImageObjectTest)
ADD_TEST(igstkUSImageObjectRepresentationTest ${IGSTK_TESTS} igstkUSImageObjectRepresentationTest)
ADD_TEST(igstkToolCalibrationTest ${IGSTK_TESTS} igstkToolCalibrationTest)
ADD_TEST(igstkBasicTrackerTest ${IGSTK_TESTS} igstkBasicTrackerTest)
ADD_TEST(igstkBinaryDataTest ${IGSTK_TESTS} igstkBinaryDataTest)
ADD_TEST(igstkCommunicationTest ${IGSTK_TESTS} igstkCommunicationTest)
ADD_TEST(igstkCTImageSpatialObjectTest ${IGSTK_TESTS} igstkCTImageSpatialObjectTest )
ADD_TEST(igstkImageReaderTest ${IGSTK_TESTS} igstkImageReaderTest )
ADD_TEST(igstkImageSpatialObjectTest ${IGSTK_TESTS} igstkImageSpatialObjectTest )
ADD_TEST(igstkLandmark3DRegistrationTest ${IGSTK_TESTS} igstkLandmark3DRegistrationTest)
ADD_TEST(igstkLandmark3DRegistrationTest2 ${IGSTK_TESTS} igstkLandmark3DRegistrationTest2)
ADD_TEST(igstkLandmark3DRegistrationErrorEstimatorTest ${IGSTK_TESTS} igstkLandmark3DRegistrationErrorEstimatorTest)
ADD_TEST(igstkMRImageSpatialObjectRepresentationTest ${IGSTK_TESTS} igstkMRImageSpatialObjectRepresentationTest )
ADD_TEST(igstkMRImageSpatialObjectTest ${IGSTK_TESTS} igstkMRImageSpatialObjectTest )          
ADD_TEST(igstkMultipleOutputTest ${IGSTK_TESTS} igstkMultipleOutputTest)
ADD_TEST(igstkPivotCalibrationTest ${IGSTK_TESTS} igstkPivotCalibrationTest)
ADD_TEST(igstkObjectRepresentationRemovalTest ${IGSTK_TESTS}
igstkObjectRepresentationRemovalTest)
ADD_TEST(igstkTransductionMacroTest ${IGSTK_TESTS} igstkTransductionMacroTest)

ADD_TEST(igstkTrackerToolReferenceTest ${IGSTK_TESTS}
igstkTrackerToolReferenceTest)

ADD_TEST(igstkPrincipalAxisCalibrationTest ${IGSTK_TESTS} igstkPrincipalAxisCalibrationTest)
ADD_TEST(igstkSpatialObjectTest ${IGSTK_TESTS} igstkSpatialObjectTest)
ADD_TEST(igstkSerialCommunicationTest ${IGSTK_TESTS} igstkSerialCommunicationTest)
ADD_TEST(igstkStateMachineErrorsTest ${IGSTK_TESTS} igstkStateMachineErrorsTest)
ADD_TEST(igstkStateMachineTest ${IGSTK_TESTS} igstkStateMachineTest)
ADD_TEST(igstkStringEventTest ${IGSTK_TESTS} igstkStringEventTest )
ADD_TEST(igstkTimeStampTest ${IGSTK_TESTS} igstkTimeStampTest)
ADD_TEST(igstkTokenTest ${IGSTK_TESTS} igstkTokenTest)
ADD_TEST(igstkTrackerToolTest ${IGSTK_TESTS} igstkTrackerToolTest)
ADD_TEST(igstkTrackerTest ${IGSTK_TESTS} igstkTrackerTest)
ADD_TEST(igstkSpatialObjectCoordinateSystemTest ${IGSTK_TESTS} igstkSpatialObjectCoordinateSystemTest)
ADD_TEST(igstkCoordinateSystemTest ${IGSTK_TESTS} igstkCoordinateSystemTest)
ADD_TEST(igstkCoordinateSystemTest2 ${IGSTK_TESTS} igstkCoordinateSystemTest2)
ADD_TEST(igstkCoordinateSystemTest3 ${IGSTK_TESTS} igstkCoordinateSystemTest3)
ADD_TEST(igstkCoordinateSystemDelegatorTest ${IGSTK_TESTS}
igstkCoordinateSystemDelegatorTest)
ADD_TEST(igstkCoordinateSystemEventTest ${IGSTK_TESTS}
igstkCoordinateSystemEventTest)
ADD_TEST(igstkSpatialObjectCoordinateSystemTest3 ${IGSTK_TESTS}
igstkSpatialObjectCoordinateSystemTest3)





ADD_TEST(igstkTransformTest ${IGSTK_TESTS} igstkTransformTest)
ADD_TEST(igstkVTKLoggerOutputTest ${IGSTK_TESTS} igstkVTKLoggerOutputTest)

#-----------------------------------------------------------------------------
# Simulation test


#-----------------------------------------------------------------------------
# Tests need data input
IF (IGSTK_DATA_ROOT) 
  
  ADD_TEST(igstkTrackerToolReferenceAndImageTest ${IGSTK_TESTS} igstkTrackerToolReferenceAndImageTest
          ${IGSTK_DATA_ROOT}/Input/E000192 )

  ADD_TEST( igstkUSImageReaderTest ${IGSTK_TESTS} igstkUSImageReaderTest 
       ${IGSTK_DATA_ROOT}/Input/USLiver)

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

  ADD_TEST( igstkMR3DImageToUS3DImageRegistrationTest ${IGSTK_TESTS} 
             igstkMR3DImageToUS3DImageRegistrationTest
             ${IGSTK_DATA_ROOT}/Input/MRLiver)

  ADD_TEST( igstkAuroraTrackerSimulatedTest
              ${IGSTK_TESTS}
              igstkAuroraTrackerSimulatedTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkAuroraTrackerSimulatedTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/Input/aurora_stream_multichan_11_11_2005.txt
              ${IGSTK_DATA_ROOT}/Input/aurora_multichan.rom )

  ADD_TEST(igstkNDICommandInterpreterSimulatedTest ${IGSTK_TESTS} igstkNDICommandInterpreterSimulatedTest)
  ADD_TEST(igstkNDICommandInterpreterStressTest ${IGSTK_TESTS} igstkNDICommandInterpreterStressTest)

  ADD_TEST( igstkPolarisTrackerSimulatedTest
              ${IGSTK_TESTS}
              igstkPolarisTrackerSimulatedTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerSimulatedTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/polaris_stream_11_05_2005.txt
              ${IGSTK_DATA_ROOT}/ta2p0003-3-120.rom )
 
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
       ${IGSTK_DATA_ROOT}/Input/E000192Mod ${IGSTK_DICOM_TEST_OUTPUT_DIR})          
ENDIF(IGSTK_DATA_ROOT)

#-----------------------------------------------------------------------------
# Tests depend on external device
IF(IGSTK_TEST_AURORA_ATTACHED)
  ADD_TEST(igstkAuroraTrackerTest
           ${IGSTK_TESTS}
           igstkAuroraTrackerTest
           ${IGSTK_TEST_OUTPUT_DIR}/igstkAuroraTrackerTestLoggerOutput.txt
           ${IGSTK_TEST_AURORA_PORT_NUMBER}
           )
  ADD_TEST(igstkAuroraTrackerTest2
           ${IGSTK_TESTS}
           igstkAuroraTrackerTest2
           ${IGSTK_TEST_OUTPUT_DIR}/igstkAuroraTrackerTest2LoggerOutput.txt
           ${IGSTK_TEST_AURORA_PORT_NUMBER}
           )

 ADD_TEST(igstkAuroraTrackerToolTest ${IGSTK_TESTS} igstkAuroraTrackerToolTest)

ENDIF(IGSTK_TEST_AURORA_ATTACHED)

IF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)
  ADD_TEST(igstkNDICommandInterpreterTest ${IGSTK_TESTS} igstkNDICommandInterpreterTest)
ENDIF(IGSTK_TEST_AURORA_ATTACHED OR IGSTK_TEST_POLARIS_ATTACHED)

IF(IGSTK_TEST_POLARIS_ATTACHED)
  ADD_TEST(igstkPolarisTrackerTest ${IGSTK_TESTS} igstkPolarisTrackerTest)

  ADD_TEST( igstkPolarisTrackerTest2
              ${IGSTK_TESTS}
              igstkPolarisTrackerTest2
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest2LoggerOutput.txt
              ${IGSTK_DATA_ROOT}/Input/Passive_4Marker_Planar_Rigid_Body_8700302.rom
              )
    ADD_TEST( igstkPolarisTrackerTest3
              ${IGSTK_TESTS}
              igstkPolarisTrackerTest3
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest3LoggerOutput.txt
              ${IGSTK_DATA_ROOT}/ta2p0003-3-120.rom
              )
    ADD_TEST( igstkPolarisTrackerTest4
              ${IGSTK_TESTS}
              igstkPolarisTrackerTest4
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest4LoggerOutput.txt
              0
              1
              )
  ADD_TEST(igstkPolarisTrackerToolTest ${IGSTK_TESTS}
igstkPolarisTrackerToolTest)
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

#-----------------------------------------------------------------------------
# Tests that depend on FLTK
IF(IGSTK_USE_FLTK)
  ADD_TEST(igstkCylinderObjectTest ${IGSTK_TESTS} igstkCylinderObjectTest)
  ADD_TEST(igstkEllipsoidObjectTest ${IGSTK_TESTS} igstkEllipsoidObjectTest)
  ADD_TEST(igstkFLTKTextBufferLogOutputTest ${IGSTK_TESTS} igstkFLTKTextBufferLogOutputTest)
  ADD_TEST(igstkFLTKTextLogOutputTest ${IGSTK_TESTS} igstkFLTKTextLogOutputTest)
  ADD_TEST(igstkViewTest ${IGSTK_TESTS} igstkViewTest)

  ADD_TEST(igstkViewRefreshRateTest ${IGSTK_TESTS} 
     igstkViewRefreshRateTest
     ${IGSTK_TEST_OUTPUT_DIR}/igstkViewRefreshRateTestLog.txt
     )

  ADD_TEST(igstkPulseGeneratorTest ${IGSTK_TESTS} igstkPulseGeneratorTest)
  ADD_TEST(igstkConeObjectTest    ${IGSTK_TESTS} igstkConeObjectTest)
  ADD_TEST(igstkBoxObjectTest     ${IGSTK_TESTS} igstkBoxObjectTest)
  ADD_TEST(igstkAxesObjectTest    ${IGSTK_TESTS} igstkAxesObjectTest)
  ADD_TEST(igstkTubeObjectTest ${IGSTK_TESTS} igstkTubeObjectTest)
  ADD_TEST(igstkMouseTrackerTest ${IGSTK_TESTS} igstkMouseTrackerTest)
  ADD_TEST(igstkUltrasoundProbeObjectTest ${IGSTK_TESTS} igstkUltrasoundProbeObjectTest)
  ADD_TEST(igstkCircularSimulatedTrackerTest ${IGSTK_TESTS} igstkCircularSimulatedTrackerTest)

  ADD_TEST( igstkSpatialObjectRepresentationVisibilityTest 
              ${IGSTK_TESTS}
              --compare ${BASELINE}/igstkSpatialObjectRepresentationVisibilityTest.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkSpatialObjectRepresentationVisibilityTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest.png
              ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest2.png
          )

  ADD_TEST( igstkSpatialObjectRepresentationVisibilityTest2 
              ${IGSTK_TESTS}
              --compare
${BASELINE}/igstkSpatialObjectRepresentationVisibilityTest2.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest2.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkSpatialObjectRepresentationVisibilityTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest.png
              ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest2.png
          )

    ADD_TEST( igstkCoordinateSystemObjectWithViewTest
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkCoordinateSystemTestScreenshot1.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkCoordinateSystemTestScreenshot1.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkCoordinateSystemObjectWithViewTest
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkCoordinateSystemTestScreenshot1.png
              )

  IF (IGSTK_DATA_ROOT)

     ADD_TEST( igstkAnnotation2DTest ${IGSTK_TESTS} igstkAnnotation2DTest
         ${IGSTK_DATA_ROOT}/Input/E000192 )

    ADD_TEST( igstkAnnotation2DTest2 ${IGSTK_TESTS}
       --compare ${IGSTK_BASELINE_DIR}/igstkAnnotation2DTest2ScreenShot.png
       ${IGSTK_TEST_OUTPUT_DIR}/igstkAnnotation2DTest2ScreenShot.png
              igstkAnnotation2DTest2
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkAnnotation2DTest2ScreenShot.png)

     ADD_TEST( igstkImageSpatialObjectRepresentationTest2 
       ${IGSTK_TESTS}
       --compare ${BASELINE}/igstkImageSpatialObjectRepresentationTest2.png 
       ${IGSTK_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest2.png 
       --toleranceIntensity 10
       --toleranceRadius 5
       --toleranceNumberOfPixels 5
       igstkImageSpatialObjectRepresentationTest2
       ${IGSTK_DATA_ROOT}/Input/E000192
       ${IGSTK_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest2.png)

    ADD_TEST( igstkImageSpatialObjectRepresentationTest3 
       ${IGSTK_TESTS}
       --compare ${BASELINE}/igstkImageSpatialObjectRepresentationTest3.png 
       ${IGSTK_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest3.png 
       --toleranceIntensity 10
       --toleranceRadius 5
       --toleranceNumberOfPixels 5
       igstkImageSpatialObjectRepresentationTest3
       ${IGSTK_DATA_ROOT}/Input/E000192Mod2
       ${IGSTK_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest3.png)

    ADD_TEST( igstkVascularNetworkReaderTest ${IGSTK_TESTS} 
          --compare ${BASELINE}/igstkVascularNetworkReaderTest.png 
          ${IGSTK_TEST_OUTPUT_DIR}/igstkVascularNetworkReaderTest.png 
          --toleranceIntensity 10
          --toleranceRadius 5
          --toleranceNumberOfPixels 5
          igstkVascularNetworkReaderTest 
          ${IGSTK_DATA_ROOT}/Input/vessel.tre
          ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre 
          ${IGSTK_TEST_OUTPUT_DIR}/igstkVascularNetworkReaderTest.png
          )        

    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest 
              ${IGSTK_TESTS}
              --compare ${BASELINE}/igstkViewScreenShot.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkCTImageSpatialObjectReadingAndRepresentationTest
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )

    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest2
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkCTImageSpatialObjectReadingAndRepresentationTest2
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )



    ADD_TEST( igstkCTImageSpatialObjectRepresentationWindowLevelTest 
              ${IGSTK_TESTS}
              --compare ${BASELINE}/igstkWindowLevelScreenShot01.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkWindowLevelScreenShot01.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkCTImageSpatialObjectRepresentationWindowLevelTest
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkWindowLevelScreenShot01.png
              )

    ADD_TEST(igstkMeshObjectTest    ${IGSTK_TESTS} igstkMeshObjectTest
              ${IGSTK_DATA_ROOT}/Input/liver.msh  
              )

    ADD_TEST(igstkMeshObjectTest2    ${IGSTK_TESTS} igstkMeshObjectTest
              ${IGSTK_DATA_ROOT}/Input/lineCells.msh  
              )
  
    ADD_TEST(igstkMeshObjectTest3    ${IGSTK_TESTS} igstkMeshObjectTest
              ${IGSTK_DATA_ROOT}/Input/tetrahedraCells.msh  
              )
  
    ADD_TEST( igstkImageSpatialObjectRepresentationTest ${IGSTK_TESTS} 
              igstkImageSpatialObjectRepresentationTest 
              ${IGSTK_DATA_ROOT}/Input/E000192)   

    ADD_TEST( igstkUltrasoundImageSimulatorTest ${IGSTK_TESTS}
             --compare ${BASELINE}/igstkUltrasoundImageSimulatorTest.png 
             ${IGSTK_TEST_OUTPUT_DIR}/igstkUltrasoundImageSimulatorTest.png 
             --toleranceIntensity 10
             --toleranceRadius 5
             --toleranceNumberOfPixels 20
             igstkUltrasoundImageSimulatorTest
             ${IGSTK_DATA_ROOT}/Input/MRLiver
             ${IGSTK_TEST_OUTPUT_DIR}/igstkUltrasoundImageSimulatorTest.png)

  ENDIF (IGSTK_DATA_ROOT)

    ADD_TEST( igstkFLTKWidgetTest ${IGSTK_TESTS} igstkFLTKWidgetTest)
  # A first test to compare the one screenshot from a FLTKWidget
    ADD_TEST( igstkFLTKWidgetTest2_1
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkFLTKWidgetTest2Screenshot1.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot1.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 10
              igstkFLTKWidgetTest2
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot1.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot2.png
              )

    # A second test to compare the second screenshot
    ADD_TEST( igstkFLTKWidgetTest2_2
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkFLTKWidgetTest2Screenshot2.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot2.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 10
              igstkFLTKWidgetTest2
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot1.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkFLTKWidgetTest2Screenshot2.png
              )

            
    ADD_TEST( igstkMouseTrackerToolTest ${IGSTK_TESTS} igstkMouseTrackerToolTest)
ENDIF(IGSTK_USE_FLTK)

IF(IGSTK_USE_MicronTracker)
    ADD_TEST( igstkMicronTrackerTest
              ${IGSTK_TESTS}
              igstkMicronTrackerTest
              ${IGSTK_DATA_ROOT}/Input/CalibrationFiles
              ${IGSTK_DATA_ROOT}/Input/MicronTracker.ini
              ${IGSTK_DATA_ROOT}/Input/Markers
              ${IGSTK_TEST_OUTPUT_DIR}/igstkMicronTrackerTestLogOutput.txt)
    SET(Tests_SRCS ${Tests_SRCS}
        igstkMicronTrackerTest.cxx)

    ADD_TEST( igstkMicronTrackerToolTest
              ${IGSTK_TESTS} igstkMicronTrackerToolTest )
    SET(Tests_SRCS ${Tests_SRCS}
        igstkMicronTrackerToolTest.cxx)

ENDIF(IGSTK_USE_MicronTracker)

IF(IGSTK_USE_Qt)
    ADD_TEST( igstkQTWidgetTest
              ${IGSTK_TESTS}
              igstkQTWidgetTest)
    ADD_TEST( igstkQTWidgetTest2
              ${IGSTK_TESTS}
              igstkQTWidgetTest2)
    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest3
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkCTImageSpatialObjectReadingAndRepresentationTest3
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )
ENDIF(IGSTK_USE_Qt)


#-----------------------------------------------------------------------------
# Set the source file
SET(BasicTests_SRCS
  igstkVesselObjectTest.cxx 
  igstkUSImageObjectTest.cxx 
  igstkUSImageObjectRepresentationTest.cxx 
  igstkToolCalibrationTest 
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

  igstkObjectRepresentationRemovalTest.cxx
  igstkTransductionMacroTest.cxx

  igstkTrackerToolReferenceTest.cxx
  igstkTrackerToolReferenceAndImageTest.cxx

  igstkPrincipalAxisCalibrationTest.cxx
  igstkSpatialObjectTest.cxx
  igstkSerialCommunicationTest.cxx
  igstkStateMachineErrorsTest.cxx
  igstkStateMachineTest.cxx
  igstkStringEventTest.cxx
  igstkTimeStampTest.cxx
  igstkTokenTest.cxx
  igstkTrackerToolTest.cxx
  igstkTrackerTest.cxx
  igstkTransformTest.cxx  
  igstkVTKLoggerOutputTest.cxx
  igstkSpatialObjectCoordinateSystemTest.cxx
  igstkCoordinateSystemTest.cxx
  igstkCoordinateSystemTest2.cxx
  igstkCoordinateSystemTest3.cxx
  igstkCoordinateSystemDelegatorTest.cxx
  igstkCoordinateSystemEventTest.cxx
  igstkSpatialObjectCoordinateSystemTest3.cxx
  )  
#-----------------------------------------------------------------------------
# Testing source file depend on external device
IF(IGSTK_TEST_AURORA_ATTACHED)
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAuroraTrackerTest.cxx
  )
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAuroraTrackerTest2.cxx)

  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAuroraTrackerToolTest.cxx
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
    igstkPolarisTrackerTest2.cxx
    igstkPolarisTrackerTest3.cxx
    igstkPolarisTrackerTest4.cxx
    igstkPolarisTrackerToolTest.cxx
  )
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

#-----------------------------------------------------------------------------
# Testing source file need data input
IF(IGSTK_DATA_ROOT)
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkUSImageReaderTest.cxx 
    igstkToolCalibrationReaderTest.cxx 
    igstkPivotCalibrationReaderTest.cxx 
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
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkAnnotation2DTest.cxx
    igstkAnnotation2DTest2.cxx
    igstkAxesObjectTest.cxx
    igstkBoxObjectTest.cxx
    igstkConeObjectTest.cxx
    igstkCylinderObjectTest.cxx
    igstkEllipsoidObjectTest.cxx
    igstkFLTKTextBufferLogOutputTest.cxx
    igstkFLTKTextLogOutputTest.cxx
    igstkPulseGeneratorTest.cxx
    igstkTubeObjectTest.cxx
    igstkUltrasoundImageSimulatorTest.cxx
    igstkViewTest.cxx
    igstkViewRefreshRateTest.cxx
    igstkMouseTrackerTest.cxx
    igstkUltrasoundProbeObjectTest.cxx
    igstkSpatialObjectRepresentationVisibilityTest.cxx
    igstkCoordinateSystemObjectWithViewTest.cxx
    igstkFLTKWidgetTest.cxx
    igstkFLTKWidgetTest2.cxx
    igstkMouseTrackerToolTest.cxx
    igstkCircularSimulatedTrackerTest.cxx
  )    
  IF(IGSTK_DATA_ROOT)
    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkImageSpatialObjectRepresentationTest2.cxx  
      igstkImageSpatialObjectRepresentationTest3.cxx
      igstkVascularNetworkReaderTest.cxx
      igstkMeshObjectTest.cxx
      igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx
      igstkCTImageSpatialObjectReadingAndRepresentationTest2.cxx
      igstkCTImageSpatialObjectRepresentationWindowLevelTest.cxx
      igstkImageSpatialObjectRepresentationTest.cxx
    ) 
  ENDIF(IGSTK_DATA_ROOT)  
ENDIF(IGSTK_USE_FLTK)

IF(IGSTK_USE_MicronTracker)  
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkMicronTrackerTest.cxx
    igstkMicronTrackerToolTest.cxx)
ENDIF(IGSTK_USE_MicronTracker)  

IF(IGSTK_USE_Qt)  
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
     igstkQTWidgetTest.cxx
     igstkQTWidgetTest2.cxx
     igstkCTImageSpatialObjectReadingAndRepresentationTest3.cxx)
ENDIF(IGSTK_USE_Qt)
 

#-----------------------------------------------------------------------------
# Add testing executables
ADD_EXECUTABLE(igstkTests igstkTests.cxx ${BasicTests_SRCS})

TARGET_LINK_LIBRARIES(igstkTests IGSTK)

ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)

ADD_EXECUTABLE(igstkVersionTest igstkVersionTest.cxx)
ADD_TEST(igstkVersionTest ${EXECUTABLE_OUTPUT_PATH}/igstkVersionTest)
TARGET_LINK_LIBRARIES(igstkVersionTest vtkCommon)

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

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.ctest.in"
               "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.ctest"
               COPYONLY @ONLY IMMEDIATE)

