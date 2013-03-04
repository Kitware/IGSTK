# ---------------------------------------------------------------------------
# IGSTKTesting
# This macro is used to add tests to IGSTK CVS. This macro is useful
# to run the same set of tests also in the Sandbox. The macro is invoked
# in the CMake list file in the Testing subdirectory.
#
# We want our tests to have maximal sensitivity while not generating a
# false positive. This can only be achieved using a per test threshold.
# 
# Optional arguments:
# EXECUTABLE_NAME(string): test executable name
# LIBRARY_NAME(string): library name ( IGSTK or IGSTKSandbox )
# SANDBOX_BUILD(boolean): differentiate between main CVS or Sandbox build
# IGSTK_DATA_ROOT(string): IGSTK testing data path
# BINARY_BUILD_DIRECTORY(STRING) : binary build directory
# IGSTK_TEST_POLARIS_ATTACHED(boolean) : Option for polaris tracker 
# IGSTK_TEST_POLARIS_PORT_NUMBER(int) : port number used by the polaris tracker
# IGSTK_TEST_AURORA_ATTACHED(boolean) : Option for aurora tracker 
# IGSTK_TEST_AURORA_PORT_NUMBER(int) : port number used by the aurora tracker
# IGSTK_TEST_LOOPBACK_ATTACHED(boolean) : Option to turn on/off loopback attachment
# IGSTK_TEST_LOOPBACK_PORT_NUMBER(int) : port number used by the loopback
# IGSTK_TEST_FLOCKOFBIRD_ATTACHED(boolean) : Option for ascension tracker 
# IGSTK_TEST_FLOCKOFBIRD_PORT_NUMBER(int) : port number used for ascension tracker
# IGSTK_TEST_MicronTracker_ATTACHED(boolean) : Option for MicronTracker 
# IGSTK_USE_OpenIGTLink : Use OpenIGTLink  
# IGSTK_USE_VideoImager : Run VideoImager Tests 
# IGSTK_TEST_InfiniTrack_ATTACHED : Us InfiniTrack tests
# IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED(boolean) : Option for Atracsys easyTrack500 attached
# IGSTK_USE_Ascension3DGTracker
# IGSTK_TEST_Ascension3DGTracker_ATTACHED
# IGSTK_USE_AXIOS3D
# IGSTK_USE_NDICertusTracker
# IGSTK_TEST_NDICertusTracker_ATTACHED
# IGSTK_USE_ArucoTracker : Run ArucoTracker tests
 

MACRO(IGSTKTesting 
      EXECUTABLE_NAME 
      LIBRARY_NAME
      SANDBOX_BUILD
      IGSTK_DATA_ROOT 
      BINARY_BUILD_DIRECTORY
      IGSTK_TEST_AURORA_ATTACHED
      IGSTK_TEST_AURORA_PORT_NUMBER
      IGSTK_TEST_POLARIS_ATTACHED
      IGSTK_TEST_POLARIS_PORT_NUMBER
      IGSTK_TEST_LOOPBACK_ATTACHED
      IGSTK_TEST_LOOPBACK_PORT_NUMBER
      IGSTK_TEST_MicronTracker_ATTACHED
      IGSTK_USE_FLTK
      IGSTK_USE_Qt
      IGSTK_USE_MicronTracker
      IGSTK_USE_KWSTYLE
      IGSTK_TEST_FLOCKOFBIRD_ATTACHED
      IGSTK_TEST_FLOCKOFBIRD_PORT_NUMBER
      IGSTK_USE_OpenIGTLink
      IGSTK_USE_VideoImager
      IGSTK_TEST_InfiniTrack_ATTACHED
      IGSTK_USE_Ascension3DGTracker
      IGSTK_TEST_Ascension3DGTracker_ATTACHED
      IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED
      IGSTK_USE_AXIOS3D
      IGSTK_USE_NDICertusTracker
      IGSTK_TEST_NDICertusTracker_ATTACHED
      IGSTK_USE_ArucoTracker
    )

SET(IGSTK_TESTS "${CXX_TEST_PATH}/${EXECUTABLE_NAME}")

IF(IGSTK_DATA_ROOT)
  SET(BASELINE ${IGSTK_DATA_ROOT}/Baseline)
ENDIF(IGSTK_DATA_ROOT)

SET(IGSTK_TEST_OUTPUT_DIR "${BINARY_BUILD_DIRECTORY}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTK_TEST_OUTPUT_DIR})

SET(IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR "${IGSTK_TEST_OUTPUT_DIR}/StateMachineDiagrams")
MAKE_DIRECTORY(${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})

SET(IGSTK_DICOM_TEST_OUTPUT_DIR "${IGSTK_TEST_OUTPUT_DIR}/DICOM")
MAKE_DIRECTORY(${IGSTK_DICOM_TEST_OUTPUT_DIR})

#-----------------------------------------------------------------------------
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
# Configure testing data to used in the example applications

CONFIGURE_FILE("${IGSTK_DATA_ROOT}/Input/TrackerConfiguration/micronConfiguration.xml.in"
              "${BINARY_BUILD_DIRECTORY}/Data/TrackerConfiguration/micronConfiguration.xml"
              @ONLY IMMEDIATE)
CONFIGURE_FILE("${IGSTK_DATA_ROOT}/Input/TrackerConfiguration/ascensionConfiguration.xml.in"
              "${BINARY_BUILD_DIRECTORY}/Data/TrackerConfiguration/ascensionConfiguration.xml"
              @ONLY IMMEDIATE)
CONFIGURE_FILE("${IGSTK_DATA_ROOT}/Input/TrackerConfiguration/polarisHybridConfiguration.xml.in"
              "${BINARY_BUILD_DIRECTORY}/Data/TrackerConfiguration/polarisHybridConfiguration.xml"
              @ONLY IMMEDIATE)

CONFIGURE_FILE("${IGSTK_DATA_ROOT}/Input/TrackerConfiguration/auroraConfiguration.xml.in"
              "${BINARY_BUILD_DIRECTORY}/Data/TrackerConfiguration/auroraConfiguration.xml"
              @ONLY IMMEDIATE)
CONFIGURE_FILE("${IGSTK_DATA_ROOT}/Input/TrackerConfiguration/vicraConfiguration.xml.in"
              "${BINARY_BUILD_DIRECTORY}/Data/TrackerConfiguration/vicraConfiguration.xml"
              @ONLY IMMEDIATE)



#-----------------------------------------------------------------------------
# Add testing command
ADD_TEST(igstkUSImageObjectTest ${IGSTK_TESTS} igstkUSImageObjectTest)
ADD_TEST(igstkUSImageObjectRepresentationTest ${IGSTK_TESTS} igstkUSImageObjectRepresentationTest)
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
ADD_TEST(igstkObjectRepresentationRemovalTest ${IGSTK_TESTS}
igstkObjectRepresentationRemovalTest)
ADD_TEST(igstkTransductionMacroTest ${IGSTK_TESTS} igstkTransductionMacroTest)

ADD_TEST(igstkTrackerToolReferenceTest ${IGSTK_TESTS}
igstkTrackerToolReferenceTest)

ADD_TEST(igstkSpatialObjectTest ${IGSTK_TESTS} igstkSpatialObjectTest)
ADD_TEST(igstkSerialCommunicationTest ${IGSTK_TESTS} igstkSerialCommunicationTest ${IGSTK_TEST_OUTPUT_DIR} )
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
ADD_TEST(igstkSimulatedTrackerTest ${IGSTK_TESTS} igstkSimulatedTrackerTest)

ADD_TEST(igstkTransformTest ${IGSTK_TESTS} igstkTransformTest)
ADD_TEST(igstkAffineTransformTest ${IGSTK_TESTS} igstkAffineTransformTest)
ADD_TEST(igstkPerspectiveTransformTest ${IGSTK_TESTS}
igstkPerspectiveTransformTest)
ADD_TEST(igstkVTKLoggerOutputTest ${IGSTK_TESTS} igstkVTKLoggerOutputTest)

ADD_TEST( igstkPivotCalibrationAlgorithmTest ${IGSTK_TESTS} igstkPivotCalibrationAlgorithmTest)

ADD_TEST( igstkPETImageSpatialObjectTest ${IGSTK_TESTS} igstkPETImageSpatialObjectTest)
ADD_TEST( igstkPETImageSpatialObjectRepresentationTest ${IGSTK_TESTS} igstkPETImageSpatialObjectRepresentationTest)

#-----------------------------------------------------------------------------
# Simulation test


#-----------------------------------------------------------------------------
# Tests need data input
IF (IGSTK_DATA_ROOT) 
  
  ADD_TEST(igstkTrackerToolReferenceAndImageTest ${IGSTK_TESTS} igstkTrackerToolReferenceAndImageTest
          ${IGSTK_DATA_ROOT}/Input/E000192 )

  ADD_TEST( igstkUSImageReaderTest ${IGSTK_TESTS} igstkUSImageReaderTest 
       ${IGSTK_DATA_ROOT}/Input/USLiver
       ${IGSTK_DATA_ROOT}/Input/MRLiver)
       
  ADD_TEST( igstkMR3DImageToUS3DImageRegistrationTest ${IGSTK_TESTS} 
             igstkMR3DImageToUS3DImageRegistrationTest
             ${IGSTK_DATA_ROOT}/Input/MRLiver)

  ADD_TEST( igstkAuroraTrackerSimulatedTest
              ${IGSTK_TESTS}
              igstkAuroraTrackerSimulatedTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkAuroraTrackerSimulatedTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/Input/aurora_stream_multichan_11_11_2005.txt
              ${IGSTK_DATA_ROOT}/Input/aurora_multichan.rom
              ${IGSTK_TEST_AURORA_PORT_NUMBER} )

IF(${IGSTK_USE_ArucoTracker})
  ADD_TEST( igstkArucoTrackerSimulatedTest
              ${IGSTK_TESTS}
              igstkArucoTrackerSimulatedTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkArucoTrackerSimulatedTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/Input/ArucoSimulationVideo.avi
			  ${IGSTK_DATA_ROOT}/Input/ArucoCalibrationFile.yml
              )
  ADD_TEST( igstkArucoTrackerToolTest
              ${IGSTK_TESTS}
              igstkArucoTrackerToolTest
              )
ENDIF(${IGSTK_USE_ArucoTracker})

  ADD_TEST(igstkNDICommandInterpreterSimulatedTest ${IGSTK_TESTS}
igstkNDICommandInterpreterSimulatedTest ${IGSTK_TEST_OUTPUT_DIR}
${IGSTK_TEST_POLARIS_PORT_NUMBER} ${IGSTK_DATA_ROOT})
  ADD_TEST(igstkNDICommandInterpreterStressTest ${IGSTK_TESTS}
igstkNDICommandInterpreterStressTest ${IGSTK_TEST_OUTPUT_DIR} ${IGSTK_DATA_ROOT}
${IGSTK_TEST_POLARIS_PORT_NUMBER})

  ADD_TEST( igstkPolarisTrackerSimulatedTest
              ${IGSTK_TESTS}
              igstkPolarisTrackerSimulatedTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerSimulatedTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/polaris_stream_11_05_2005.txt
              ${IGSTK_DATA_ROOT}/ta2p0003-3-120.rom 
              ${IGSTK_TEST_POLARIS_PORT_NUMBER})

  ADD_TEST( igstkPolarisClassicTrackerSimulatedTest
              ${IGSTK_TESTS}
              igstkPolarisClassicTrackerSimulatedTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisClassicTrackerSimulatedTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/polaris_classic_stream.txt
              ${IGSTK_TEST_POLARIS_PORT_NUMBER})
 
  ADD_TEST(igstkSerialCommunicationSimulatorTest ${IGSTK_TESTS}
igstkSerialCommunicationSimulatorTest ${IGSTK_TEST_OUTPUT_DIR}  
              ${IGSTK_DATA_ROOT}/polaris_stream_11_05_2005.txt} )
  
  ADD_TEST( igstkDICOMImageReaderTest ${IGSTK_TESTS} igstkDICOMImageReaderTest 
       ${IGSTK_DATA_ROOT}/Input/E000192 )
  ADD_TEST( igstkCTImageReaderTest ${IGSTK_TESTS} igstkCTImageReaderTest 
       ${IGSTK_DATA_ROOT}/Input/E000192
       ${IGSTK_DATA_ROOT}/Input/MRLiver)
  ADD_TEST( igstkMRImageReaderTest ${IGSTK_TESTS} igstkMRImageReaderTest 
       ${IGSTK_DATA_ROOT}/Input/MRLiver
       ${IGSTK_DATA_ROOT}/Input/E000192 )
  ADD_TEST( igstkMeshReaderTest ${IGSTK_TESTS} igstkMeshReaderTest 
       ${IGSTK_DATA_ROOT}/Input/liver.msh  
       ${IGSTK_DATA_ROOT}/Input/liverCorruptedOnPurpose.msh )
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
  ADD_TEST( igstkReslicerPlaneSpatialObjectTest ${IGSTK_TESTS} 
          igstkReslicerPlaneSpatialObjectTest
          ${IGSTK_DATA_ROOT}/Input/E000192
        )

  ADD_TEST( igstkPETImageReaderTest ${IGSTK_TESTS} igstkPETImageReaderTest
   ${IGSTK_DATA_ROOT}/Input/PET
   ${IGSTK_DATA_ROOT}/Input/MRLiver
)

  IF(${IGSTK_USE_AXIOS3D})
    ADD_TEST( igstkAxios3DTrackerSimulatedTest ${IGSTK_TESTS}
          igstkAxios3DTrackerSimulatedTest
          ${IGSTK_DATA_ROOT}/AXIOS3DsimulationData/Locators.xml
          ${IGSTK_DATA_ROOT}/AXIOS3DsimulationData
          ${IGSTK_TEST_OUTPUT_DIR}/igstkAxios3DTrackerSimulatedTestLoggerOutput.txt
    )
	ADD_TEST( igstkAxios3DTrackerToolTest ${IGSTK_TESTS}
          igstkAxios3DTrackerToolTest
    )
  ENDIF(${IGSTK_USE_AXIOS3D})

ENDIF(IGSTK_DATA_ROOT)

#-----------------------------------------------------------------------------
# Tests depend on external device
IF(${IGSTK_TEST_AURORA_ATTACHED})
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

ENDIF(${IGSTK_TEST_AURORA_ATTACHED})

IF(${IGSTK_TEST_AURORA_ATTACHED} OR ${IGSTK_TEST_POLARIS_ATTACHED})
  ADD_TEST(igstkNDICommandInterpreterTest ${IGSTK_TESTS}
igstkNDICommandInterpreterTest ${IGSTK_TEST_OUTPUT_DIR}
${IGSTK_TEST_POLARIS_PORT_NUMBER} ${IGSTK_DATA_ROOT})
ENDIF(${IGSTK_TEST_AURORA_ATTACHED} OR ${IGSTK_TEST_POLARIS_ATTACHED})

IF(${IGSTK_TEST_POLARIS_ATTACHED})
  ADD_TEST(igstkPolarisTrackerTest ${IGSTK_TESTS} igstkPolarisTrackerTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/Input/Passive_4Marker_Planar_Rigid_Body_8700302.rom
              ${IGSTK_TEST_POLARIS_PORT_NUMBER}
          )

  ADD_TEST( igstkPolarisTrackerTest2
              ${IGSTK_TESTS}
              igstkPolarisTrackerTest2
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest2LoggerOutput.txt
              ${IGSTK_DATA_ROOT}/Input/Passive_4Marker_Planar_Rigid_Body_8700302.rom
              ${IGSTK_TEST_POLARIS_PORT_NUMBER}
              )
    ADD_TEST( igstkPolarisTrackerTest3
              ${IGSTK_TESTS}
              igstkPolarisTrackerTest3
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest3LoggerOutput.txt
              ${IGSTK_DATA_ROOT}/ta2p0003-3-120.rom
              ${IGSTK_TEST_POLARIS_PORT_NUMBER}
              )
    ADD_TEST( igstkPolarisTrackerTest4
              ${IGSTK_TESTS}
              igstkPolarisTrackerTest4
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerTest4LoggerOutput.txt
              ${IGSTK_TEST_POLARIS_PORT_NUMBER}
              0
              1
              )
  ADD_TEST(igstkPolarisTrackerToolTest ${IGSTK_TESTS} igstkPolarisTrackerToolTest 0)
ENDIF(${IGSTK_TEST_POLARIS_ATTACHED})

IF(${IGSTK_TEST_FLOCKOFBIRD_ATTACHED})
    ADD_TEST( igstkFlockOfBirdsTrackerTest2
              ${IGSTK_TESTS}
              igstkAscensionTrackerTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkAscensionTestLoggerOutput.txt
              ${IGSTK_TEST_FLOCKOFBIRD_PORT_NUMBER}
              )
ENDIF(${IGSTK_TEST_FLOCKOFBIRD_ATTACHED})

IF(${IGSTK_TEST_NDICertusTracker_ATTACHED})
  ADD_TEST(igstkNDICertusTrackerTest ${IGSTK_TESTS} igstkNDICertusTrackerTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkPolarisTrackerTestLoggerOutput.txt
              ${IGSTK_DATA_ROOT}/Input/Passive_4Marker_Planar_Rigid_Body_8700302.rom
              ${IGSTK_TEST_POLARIS_PORT_NUMBER}
          )
ENDIF(${IGSTK_TEST_NDICertusTracker_ATTACHED})



#-----------------------------------------------------------------------------
# Tests that depend on FLTK
IF(${IGSTK_USE_FLTK})
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
  ADD_TEST(igstkMeshObjectTest    ${IGSTK_TESTS} igstkMeshObjectTest )
  ADD_TEST(igstkMouseTrackerTest ${IGSTK_TESTS} igstkMouseTrackerTest)
  ADD_TEST(igstkUltrasoundProbeObjectTest ${IGSTK_TESTS} igstkUltrasoundProbeObjectTest)
  ADD_TEST(igstkCircularSimulatedTrackerTest ${IGSTK_TESTS} igstkCircularSimulatedTrackerTest)

  ADD_TEST( igstkSpatialObjectRepresentationVisibilityTest 
              ${IGSTK_TESTS}
              --compare ${BASELINE}/igstkSpatialObjectRepresentationVisibilityTest.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest.png 
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000 
              igstkSpatialObjectRepresentationVisibilityTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest.png
              ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTestb.png
          )

  ADD_TEST( igstkSpatialObjectRepresentationVisibilityTest2 
              ${IGSTK_TESTS}
              --compare ${BASELINE}/igstkSpatialObjectRepresentationVisibilityTest2.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest2b.png 
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000 
              igstkSpatialObjectRepresentationVisibilityTest
              ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest2.png
              ${IGSTK_TEST_OUTPUT_DIR}/igstkSpatialObjectRepresentationVisibilityTest2b.png
          )

    ADD_TEST( igstkCoordinateSystemObjectWithViewTest
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkCoordinateSystemTestScreenshot1.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkCoordinateSystemTestScreenshot1.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 30
              igstkCoordinateSystemObjectWithViewTest
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkCoordinateSystemTestScreenshot1.png
              )

  IF (IGSTK_DATA_ROOT)

     ADD_TEST( igstkAnnotation2DTest ${IGSTK_TESTS} igstkAnnotation2DTest
         ${IGSTK_DATA_ROOT}/Input/E000192 )

    ADD_TEST( igstkAnnotation2DTest2 ${IGSTK_TESTS}
       --compare ${BASELINE}/igstkAnnotation2DTest2ScreenShot.png
       ${IGSTK_TEST_OUTPUT_DIR}/igstkAnnotation2DTest2ScreenShot.png
       --toleranceIntensity 10
       --toleranceRadius 10
       --toleranceNumberOfPixels 1000
              igstkAnnotation2DTest2
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkAnnotation2DTest2ScreenShot.png)

     ADD_TEST( igstkImageSpatialObjectRepresentationTest2 
       ${IGSTK_TESTS}
       --compare ${BASELINE}/igstkImageSpatialObjectRepresentationTest2.png 
       ${IGSTK_TEST_OUTPUT_DIR}/igstkImageSpatialObjectRepresentationTest2.png 
       --toleranceIntensity 10
       --toleranceRadius 5
       --toleranceNumberOfPixels 25
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

   ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest 
              ${IGSTK_TESTS}
              --compare ${BASELINE}/igstkViewScreenShot1.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot1.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkCTImageSpatialObjectReadingAndRepresentationTest
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot1.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )

    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest2
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot2.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot2.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkCTImageSpatialObjectReadingAndRepresentationTest2
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot2.png
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

    ADD_TEST(igstkMeshObjectTestLiver    ${IGSTK_TESTS} 
              --compare ${BASELINE}/igstkMeshLiver.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkMeshLiver.png 
              --toleranceIntensity 20
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkMeshObjectTest2
              ${IGSTK_DATA_ROOT}/Input/liver.msh  
              ${IGSTK_TEST_OUTPUT_DIR}/igstkMeshLiver.png
              )

    ADD_TEST(igstkMeshObjectTestLineCells ${IGSTK_TESTS}
              --compare ${BASELINE}/igstkMeshLineCells.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkMeshLineCells.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkMeshObjectTest2
              ${IGSTK_DATA_ROOT}/Input/lineCells.msh  
              ${IGSTK_TEST_OUTPUT_DIR}/igstkMeshLineCells.png
              )
  
    ADD_TEST(igstkMeshObjectTestTetrCells ${IGSTK_TESTS} 
              --compare ${BASELINE}/igstkMeshTetrahedraCells.png 
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkMeshTetrahedraCells.png 
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5 
              igstkMeshObjectTest2
              ${IGSTK_DATA_ROOT}/Input/tetrahedraCells.msh
              ${IGSTK_TEST_OUTPUT_DIR}/igstkMeshTetrahedraCells.png  
              )
  
    ADD_TEST( igstkImageSpatialObjectRepresentationTest ${IGSTK_TESTS} 
              igstkImageSpatialObjectRepresentationTest 
              ${IGSTK_DATA_ROOT}/Input/E000192)   

    ADD_TEST( igstkUltrasoundImageSimulatorTest ${IGSTK_TESTS}
             --compare ${BASELINE}/igstkUltrasoundImageSimulatorTest.png 
             ${IGSTK_TEST_OUTPUT_DIR}/igstkUltrasoundImageSimulatorTest.png 
             --toleranceIntensity 20
             --toleranceRadius 10
             --toleranceNumberOfPixels 1000
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


ADD_TEST( igstkImageResliceObjectRepresentationFltkTest
           ${IGSTK_TESTS}
           --compare
           ${IGSTK_DATA_ROOT}/Baseline/igstkImageResliceObjectRepresentationFltkTest.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationFltkTest.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkImageResliceObjectRepresentationFltkTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationFltkTest.png)

ADD_TEST( igstkImageResliceObjectRepresentationFltkTest2
           ${IGSTK_TESTS}
           --compare
           ${IGSTK_DATA_ROOT}/Baseline//igstkImageResliceObjectRepresentationFltkTest2.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationFltkTest2.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkImageResliceObjectRepresentationFltkTest2
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationFltkTest2.png)


  ADD_TEST( igstkImageResliceObjectRepresentationFltkTest3
      ${IGSTK_TESTS}
           --compare
           ${IGSTK_DATA_ROOT}/Baseline/igstkImageResliceObjectRepresentationFltkTest3.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationFltkTest3.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkImageResliceObjectRepresentationFltkTest3
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_DATA_ROOT}/Input/TrackerToolRepresentationMeshes/tool.msh
              ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationFltkTest3.png
         )

  ADD_TEST( igstkCrossHairSpatialObjectTest
              ${IGSTK_TESTS}
              --compare
              ${IGSTK_DATA_ROOT}/Baseline/igstkCrossHairSpatialObjectTest.png                       
              ${IGSTK_TEST_OUTPUT_DIR}/igstkCrossHairSpatialObjectTest.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 3000
              igstkCrossHairSpatialObjectTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_TEST_OUTPUT_DIR}/igstkCrossHairSpatialObjectTest.png)
 
  ADD_TEST( igstkCrossHairObjectRepresentationTest
              ${IGSTK_TESTS}
              --compare
              ${IGSTK_DATA_ROOT}/Baseline/igstkCrossHairObjectRepresentationTest.png                       
              ${IGSTK_TEST_OUTPUT_DIR}/igstkCrossHairObjectRepresentationTest.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 3000
              igstkCrossHairObjectRepresentationTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_TEST_OUTPUT_DIR}/igstkCrossHairObjectRepresentationTest.png)
   
ENDIF(${IGSTK_USE_FLTK})

IF(${IGSTK_TEST_MicronTracker_ATTACHED})
    ADD_TEST( igstkMicronTrackerTest
              ${IGSTK_TESTS}
              igstkMicronTrackerTest
              ${IGSTK_DATA_ROOT}/Input/CalibrationFiles
              ${IGSTK_DATA_ROOT}/Input/MicronTracker.ini
              ${IGSTK_DATA_ROOT}/Input/Markers
              ${IGSTK_TEST_OUTPUT_DIR}/igstkMicronTrackerTestLogOutput.txt)
    ADD_TEST( igstkMicronTrackerToolTest
              ${IGSTK_TESTS} igstkMicronTrackerToolTest )
ENDIF(${IGSTK_TEST_MicronTracker_ATTACHED})

IF(${IGSTK_TEST_InfiniTrack_ATTACHED})
    ADD_TEST( igstkInfiniTrackTrackerTest
              ${IGSTK_TESTS}
              igstkInfiniTrackTrackerTest
              ${IGSTK_DATA_ROOT}/Input/InfiniMarkers
              ${IGSTK_TEST_OUTPUT_DIR}/igstkInfiniTrackTrackerTestLogOutput.txt)
    ADD_TEST( igstkInfiniTrackTrackerToolTest
              ${IGSTK_TESTS} igstkInfiniTrackTrackerToolTest )
ENDIF(${IGSTK_TEST_InfiniTrack_ATTACHED})

           
IF( ${IGSTK_TEST_Ascension3DGTracker_ATTACHED} )
  ADD_TEST( igstkAscension3DGTrackerToolTest ${IGSTK_TESTS} igstkAscension3DGTrackerToolTest )
  ADD_TEST( igstkAscension3DGTrackerTest ${IGSTK_TESTS} igstkAscension3DGTrackerTest
            ${IGSTK_TEST_OUTPUT_DIR}/igstkAscension3DGTrackerTestLogOutput.txt  )
ENDIF (${IGSTK_TEST_Ascension3DGTracker_ATTACHED})

IF( ${IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED} )
  ADD_TEST( igstkAtracsysEasyTrackTrackerTest ${IGSTK_TESTS} igstkAtracsysEasyTrackTrackerTest
            ${IGSTK_TEST_OUTPUT_DIR}/igstkAtracsysEasyTrackTrackerTestLogOutput.txt  )
  ADD_TEST( igstkAtracsysEasyTrackTrackerToolTest ${IGSTK_TESTS} igstkAtracsysEasyTrackTrackerToolTest
            )
ENDIF (${IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED})

IF(${IGSTK_USE_OpenIGTLink})
  
  ADD_TEST( igstkTrackerToolObserverToOpenIGTLinkRelayTest
      ${IGSTK_TESTS}
      igstkTrackerToolObserverToOpenIGTLinkRelayTest
      localhost 16666 1000 100 10 
      )
 
  IF (${IGSTK_TEST_AURORA_ATTACHED})
  ADD_TEST( igstkAuroraTrackerToolObserverToOpenIGTLinkRelayTest
      ${IGSTK_TESTS}
      igstkAuroraTrackerToolObserverToOpenIGTLinkRelayTest
      localhost 16666 1000 ${IGSTK_TEST_AURORA_PORT_NUMBER} 10
      )
  ENDIF (${IGSTK_TEST_AURORA_ATTACHED})
  
  IF(${IGSTK_TEST_MicronTracker_ATTACHED})
    ADD_TEST( igstkMicronTrackerToolObserverToOpenIGTLinkRelayTest
      ${IGSTK_TESTS}
      igstkMicronTrackerToolObserverToOpenIGTLinkRelayTest
      localhost 16666 1000 
      ${IGSTK_DATA_ROOT}/Input/CalibrationFiles
      ${IGSTK_DATA_ROOT}/Input/MicronTracker.ini
      ${IGSTK_DATA_ROOT}/Input/Markers )
  ENDIF(${IGSTK_TEST_MicronTracker_ATTACHED})

ENDIF(${IGSTK_USE_OpenIGTLink})  

IF(${IGSTK_USE_Qt})
    ADD_TEST( igstkQTWidgetTest
              ${IGSTK_TESTS}
              igstkQTWidgetTest)
    ADD_TEST( igstkQTWidgetTest2
              ${IGSTK_TESTS}
              igstkQTWidgetTest2)
    ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest3
              ${IGSTK_TESTS}
              --compare ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot3.png
                        ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot3.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkCTImageSpatialObjectReadingAndRepresentationTest3
              ${IGSTK_DATA_ROOT}/Input/E000192
              ${IGSTK_TEST_OUTPUT_DIR}/igstkViewScreenShot3.png
              ${IGSTK_DATA_ROOT}/Input/E000192Excerpt )

    ADD_TEST( igstkImageResliceObjectRepresentationQtTest
              ${IGSTK_TESTS}
              --compare
              ${IGSTK_DATA_ROOT}/Baseline/igstkImageResliceObjectRepresentationQtTest.png
              ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationQtTest.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkImageResliceObjectRepresentationQtTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationQtTest.png)

    ADD_TEST( igstkImageResliceObjectRepresentationQtTest2
              ${IGSTK_TESTS}
              --compare
              ${IGSTK_DATA_ROOT}/Baseline/igstkImageResliceObjectRepresentationQtTest2.png
              ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationQtTest2.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkImageResliceObjectRepresentationQtTest2
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationQtTest2.png)


    ADD_TEST( igstkImageResliceObjectRepresentationQtTest3
              ${IGSTK_TESTS}
              --compare
            ${IGSTK_DATA_ROOT}/Baseline/igstkImageResliceObjectRepresentationQtTest3.png
            ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationQtTest3.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkImageResliceObjectRepresentationQtTest3
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_DATA_ROOT}/Input/TrackerToolRepresentationMeshes/tool.msh
              ${IGSTK_TEST_OUTPUT_DIR}/igstkImageResliceObjectRepresentationQtTest3.png)

  ADD_TEST( igstkToolProjectionSpatialObjectQtTest
              ${IGSTK_TESTS}
              --compare
              ${IGSTK_DATA_ROOT}/Baseline/igstkToolProjectionSpatialObjectQtTest.png                       
              ${IGSTK_TEST_OUTPUT_DIR}/igstkToolProjectionSpatialObjectQtTest.png
              --toleranceIntensity 10
              --toleranceRadius    20
              --toleranceNumberOfPixels 500
              igstkToolProjectionSpatialObjectQtTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_DATA_ROOT}/Input/TrackerToolRepresentationMeshes/tool.msh
              ${IGSTK_TEST_OUTPUT_DIR}/igstkToolProjectionSpatialObjectQtTest.png)

  ADD_TEST( igstkToolProjectionSpatialObjectQtTest2
              ${IGSTK_TESTS}
              --compare
              ${IGSTK_DATA_ROOT}/Baseline/igstkToolProjectionSpatialObjectQtTest2.png                       
              ${IGSTK_TEST_OUTPUT_DIR}/igstkToolProjectionSpatialObjectQtTest2.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkToolProjectionSpatialObjectQtTest2
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTK_DATA_ROOT}/Input/TrackerToolRepresentationMeshes/tool.msh
              ${IGSTK_TEST_OUTPUT_DIR}/igstkToolProjectionSpatialObjectQtTest2.png)
 
  ADD_TEST( igstkMeshResliceObjectRepresentationQtTest
              ${IGSTK_TESTS}
              --compare
              ${IGSTK_DATA_ROOT}/Baseline/igstkMeshResliceObjectRepresentationQtTest.png                       
              ${IGSTK_TEST_OUTPUT_DIR}/igstkMeshResliceObjectRepresentationQtTest.png
              --toleranceIntensity 10
              --toleranceRadius    10
              --toleranceNumberOfPixels 1000
              igstkMeshResliceObjectRepresentationQtTest
              ${IGSTK_DATA_ROOT}/Input/TrackerWorkingVolumeMeshes/ascension_mrt_volume.msh 
              ${IGSTK_DATA_ROOT}/Input/TrackerToolRepresentationMeshes/tool.msh
              ${IGSTK_TEST_OUTPUT_DIR}/igstkMeshResliceObjectRepresentationQtTest.png)


ENDIF(${IGSTK_USE_Qt})

IF(${IGSTK_USE_VideoImager})

  ADD_TEST( igstkVideoImagerTest
      ${IGSTK_TESTS}
      igstkVideoImagerTest
      )

  ADD_TEST( igstkVideoImagerToolTest
      ${IGSTK_TESTS}
      igstkVideoImagerToolTest
      )

  ADD_TEST( igstkFrameTest
      ${IGSTK_TESTS}
      igstkFrameTest
      )

  ADD_TEST( igstkVideoFrameSpatialObjectTest
      ${IGSTK_TESTS}
      igstkVideoFrameSpatialObjectTest
      )

  ADD_TEST( igstkVideoFrameRepresentationTest
      ${IGSTK_TESTS}
      igstkVideoFrameRepresentationTest
      )

ENDIF(${IGSTK_USE_VideoImager})
 

#-----------------------------------------------------------------------------
# Set the source file
SET(BasicTests_SRCS
  igstkUSImageObjectTest.cxx 
  igstkUSImageObjectRepresentationTest.cxx 
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

  igstkObjectRepresentationRemovalTest.cxx
  igstkTransductionMacroTest.cxx

  igstkTrackerToolReferenceTest.cxx
  igstkTrackerToolReferenceAndImageTest.cxx

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
  igstkSimulatedTrackerTest.cxx
  
  igstkDefaultWidget.cxx

  igstkAffineTransformTest.cxx
  igstkPerspectiveTransformTest.cxx

  igstkReslicerPlaneSpatialObjectTest.cxx

  igstkPivotCalibrationAlgorithmTest.cxx
  
  igstkPETImageSpatialObjectTest.cxx
  igstkPETImageSpatialObjectRepresentationTest.cxx

  )  
#-----------------------------------------------------------------------------
# Testing source file depend on external device
IF(${IGSTK_TEST_AURORA_ATTACHED})
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAuroraTrackerTest.cxx
  )
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAuroraTrackerTest2.cxx)

  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAuroraTrackerToolTest.cxx
  )
 
ENDIF(${IGSTK_TEST_AURORA_ATTACHED})

IF(${IGSTK_TEST_AURORA_ATTACHED} OR ${IGSTK_TEST_POLARIS_ATTACHED})
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkNDICommandInterpreterTest.cxx
  )
ENDIF(${IGSTK_TEST_AURORA_ATTACHED} OR ${IGSTK_TEST_POLARIS_ATTACHED})

IF(${IGSTK_TEST_POLARIS_ATTACHED})
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkPolarisTrackerTest.cxx
    igstkPolarisTrackerTest2.cxx
    igstkPolarisTrackerTest3.cxx
    igstkPolarisTrackerTest4.cxx
    igstkPolarisTrackerToolTest.cxx
  )
ENDIF(${IGSTK_TEST_POLARIS_ATTACHED})

IF(${IGSTK_TEST_FLOCKOFBIRD_ATTACHED})
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAscensionTrackerTest.cxx
  )
ENDIF(${IGSTK_TEST_FLOCKOFBIRD_ATTACHED})

IF(${IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED})
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkAtracsysEasyTrackTrackerTest.cxx
	igstkAtracsysEasyTrackTrackerToolTest.cxx
  )
ENDIF(${IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED})


#-----------------------------------------------------------------------------
# Testing source file need data input
IF(IGSTK_DATA_ROOT)
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkUSImageReaderTest.cxx 
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
    igstkPETImageReaderTest.cxx
    igstkPolarisClassicTrackerSimulatedTest.cxx
  )

  IF(${IGSTK_USE_AXIOS3D})
    SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkAxios3DTrackerSimulatedTest.cxx
	igstkAxios3DTrackerToolTest.cxx)
  ENDIF(${IGSTK_USE_AXIOS3D})
  
  IF(${IGSTK_USE_ArucoTracker})
    SET(BasicTests_SRCS
    ${BasicTests_SRCS}
	  igstkArucoTrackerSimulatedTest.cxx
	  igstkArucoTrackerToolTest.cxx)
  ENDIF(${IGSTK_USE_ArucoTracker})

ENDIF(IGSTK_DATA_ROOT)

#-----------------------------------------------------------------------------
# Testing source file depend on FLTK
IF(${IGSTK_USE_FLTK})  
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
    igstkMeshObjectTest.cxx
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
      igstkMeshObjectTest2.cxx
      igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx
      igstkCTImageSpatialObjectReadingAndRepresentationTest2.cxx
      igstkCTImageSpatialObjectRepresentationWindowLevelTest.cxx
      igstkImageSpatialObjectRepresentationTest.cxx

      igstkImageResliceObjectRepresentationFltkTest.cxx
      igstkImageResliceObjectRepresentationFltkTest2.cxx
      igstkImageResliceObjectRepresentationFltkTest3.cxx

      igstkCrossHairSpatialObjectTest.cxx
      igstkCrossHairObjectRepresentationTest.cxx
    ) 
  ENDIF(IGSTK_DATA_ROOT)  
ENDIF(${IGSTK_USE_FLTK})

IF(${IGSTK_USE_MicronTracker})  
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkMicronTrackerTest.cxx
    igstkMicronTrackerToolTest.cxx)
ENDIF(${IGSTK_USE_MicronTracker})  

IF(${IGSTK_TEST_InfiniTrack_ATTACHED})  
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkInfiniTrackTrackerTest.cxx
    igstkInfiniTrackTrackerToolTest.cxx)
ENDIF(${IGSTK_TEST_InfiniTrack_ATTACHED})  

IF(${IGSTK_USE_Ascension3DGTracker})  
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkAscension3DGTrackerTest.cxx
    igstkAscension3DGTrackerToolTest.cxx
    )
ENDIF(${IGSTK_USE_Ascension3DGTracker})  

IF(${IGSTK_USE_NDICertusTracker})  
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
    igstkNDICertusTrackerTest.cxx
    igstkNDICertusTrackerToolTest.cxx
    )
ENDIF(${IGSTK_USE_NDICertusTracker})  



IF(${IGSTK_USE_Qt})  
  SET(BasicTests_SRCS
    ${BasicTests_SRCS}
     igstkQTWidgetTest.cxx
     igstkQTWidgetTest2.cxx
     igstkCTImageSpatialObjectReadingAndRepresentationTest3.cxx
     igstkImageResliceObjectRepresentationQtTest.cxx
     igstkImageResliceObjectRepresentationQtTest2.cxx
     igstkImageResliceObjectRepresentationQtTest3.cxx
     )

  IF(IGSTK_DATA_ROOT)
    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
     igstkToolProjectionSpatialObjectQtTest.cxx
     igstkToolProjectionSpatialObjectQtTest2.cxx
     igstkMeshResliceObjectRepresentationQtTest.cxx
    ) 
  ENDIF(IGSTK_DATA_ROOT)  

ENDIF(${IGSTK_USE_Qt})
 
IF(${IGSTK_USE_OpenIGTLink})

    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkTrackerToolObserverToOpenIGTLinkRelayTest.cxx
      )

    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkAuroraTrackerToolObserverToOpenIGTLinkRelayTest.cxx
      )

  IF(${IGSTK_USE_MicronTracker})
     SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkMicronTrackerToolObserverToOpenIGTLinkRelayTest.cxx
      )
  ENDIF(${IGSTK_USE_MicronTracker})

ENDIF(${IGSTK_USE_OpenIGTLink})  


IF(${IGSTK_USE_VideoImager})
    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkVideoImagerTest.cxx
      )
    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkVideoImagerToolTest.cxx
      )
    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkFrameTest.cxx
      )
    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkVideoFrameSpatialObjectTest.cxx
      )
    SET(BasicTests_SRCS
      ${BasicTests_SRCS}
      igstkVideoFrameRepresentationTest.cxx
      )
ENDIF(${IGSTK_USE_VideoImager})
 
IF(${SANDBOX_BUILD})
  FOREACH(SourceFile ${BasicTests_SRCS})
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
# Copy the .NoDartCoverage file to the duplicate testing directory
 CONFIGURE_FILE( ${IGSTK_SOURCE_DIR}/Testing/.NoDartCoverage ${IGSTK_TESTS_LATEST}/.NoDartCoverage COPYONLY)
 SOURCE_GROUP(TestsModified FILES ${TestsModifiedFiles})
 ADD_EXECUTABLE(${EXECUTABLE_NAME} ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Testing/igstkTests.cxx ${IGSTKLatestTests_SRCS})
ELSE(${SANDBOX_BUILD})
  ADD_EXECUTABLE(${EXECUTABLE_NAME} igstkTests.cxx ${BasicTests_SRCS})
  ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
  ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
  TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon ITKCommon)

  ADD_EXECUTABLE(igstkVersionTest igstkVersionTest.cxx)
  ADD_TEST(igstkVersionTest ${EXECUTABLE_OUTPUT_PATH}/igstkVersionTest)
  TARGET_LINK_LIBRARIES(igstkVersionTest vtkCommon ITKCommon)
  ADD_EXECUTABLE(igstkStateMachineExportTest igstkStateMachineExportTest.cxx)
  ADD_TEST(igstkStateMachineExportTest ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
  TARGET_LINK_LIBRARIES(igstkStateMachineExportTest ${LIBRARY_NAME})
ENDIF(${SANDBOX_BUILD})

TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} ${LIBRARY_NAME})

#-----------------------------------------------------------------------------
# Configure a header needed by igstkSystemInformation.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/igstkSystemInformation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/igstkSystemInformation.h"
               @ONLY IMMEDIATE)

IF(${IGSTK_USE_KWSTYLE})
  ADD_TEST(KWStyle ${KWSTYLE_EXECUTABLE} ${KWSTYLE_ARGUMENTS})
ENDIF(${IGSTK_USE_KWSTYLE})

ENDMACRO(IGSTKTesting)
