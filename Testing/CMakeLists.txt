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

# Options for devices attached to serial port for testing
OPTION(IGSTKSandbox_TEST_AURORA_ATTACHED "Aurora is attached to computer" OFF)
SET( IGSTKSandbox_TEST_AURORA_PORT_NUMBER 0 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
OPTION(IGSTKSandbox_TEST_POLARIS_ATTACHED "Polaris is attached to computer" OFF)
SET( IGSTKSandbox_TEST_POLARIS_PORT_NUMBER 0 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
OPTION(IGSTKSandbox_TEST_LOOPBACK_ATTACHED "Serial Loopback is attached" OFF)
SET( IGSTKSandbox_TEST_LOOPBACK_PORT_NUMBER 1 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
OPTION(IGSTKSandbox_TEST_FLOCKOFBIRD_ATTACHED "Serial Loopback is attached" OFF)
SET( IGSTKSandbox_TEST_FLOCKOFBIRD_PORT_NUMBER 1 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
SET(IGSTK_DATA_ROOT ${IGSTK_SOURCE_DIR}/Testing/Data)

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

IF(DOT)
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

ENDIF(DOT)


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


# Add NEW Sandbox tests 
SET(SandboxTests_SRCS
# FIXME: comment out the FlockOfBirds tests just for the
# time being. It requires major work to make it consistent
# with the new Tracker API
#
#  igstkFlockOfBirdsCommandInterpreterTest.cxx
#  igstkFlockOfBirdsCommandInterpreterFBBTest.cxx

  igstkAscensionTrackerTest.cxx

  igstkGroupObjectTest.cxx
  igstkVesselObjectTest.cxx
  igstkPivotCalibrationAlgorithmTest.cxx
  igstkDICOMGenericImageReaderTest.cxx
  igstkGenericImageSpatialObjectTest.cxx
  igstkLandmarkUltrasoundCalibrationTest.cxx
  igstkSocketCommunicationTest.cxx
  igstkVascularNetworkObjectTest.cxx

  igstkPivotCalibrationTest.cxx
  igstkPrincipalAxisCalibrationTest.cxx
  igstkToolCalibrationTest.cxx
  igstkToolCalibrationReaderTest.cxx
  igstkPivotCalibrationReaderTest.cxx

  igstkDefaultWidget.h
  igstkDefaultWidget.cxx

  igstkReslicerPlaneSpatialObjectTest.cxx
  igstkAffineTransformTest.cxx
  igstkPerspectiveTransformTest.cxx

)

IF(IGSTKSandbox_USE_Qt)
  SET(SandboxTests_SRCS
    ${SandboxTests_SRCS}
  igstkImageResliceObjectRepresentationQtTest.cxx)
 SET(SandboxTests_SRCS
    ${SandboxTests_SRCS}
  igstkImageResliceObjectRepresentationQtTest2.cxx)

 SET(SandboxTests_SRCS
    ${SandboxTests_SRCS}
  igstkImageResliceObjectRepresentationQtTest3.cxx)

ENDIF(IGSTKSandbox_USE_Qt)
 
IF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)
  SET(SandboxTests_SRCS
    ${SandboxTests_SRCS}
    igstkTrackerToolObserverToSocketRelayTest.cxx
    igstkPolarisTrackerToolObserverToSocketRelayTest.cxx
    igstkAuroraTrackerToolObserverToSocketRelayTest.cxx
    igstkVicraTrackerToolObserverToSocketRelayTest.cxx
    igstkTransformSocketListenerTest.cxx
    )
  IF(IGSTKSandbox_USE_OpenIGTLink)
    SET(SandboxTests_SRCS
      ${SandboxTests_SRCS}
      igstkAuroraTrackerToolObserverToOpenIGTLinkRelayTest.cxx
      )
  ENDIF(IGSTKSandbox_USE_OpenIGTLink)  

  IF(IGSTKSandbox_USE_MicronTracker)
     SET(SandboxTests_SRCS
      ${SandboxTests_SRCS}
      igstkMicronTrackerToolObserverToSocketRelayTest.cxx
      igstkMicronTrackerToolObserverToOpenIGTLinkRelayTest.cxx
      )
  ENDIF(IGSTKSandbox_USE_MicronTracker)

ENDIF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)

ADD_TEST( igstkAffineTransformTest  ${IGSTKSandbox_TESTS} igstkAffineTransformTest )
ADD_TEST( igstkPerspectiveTransformTest  ${IGSTKSandbox_TESTS} igstkPerspectiveTransformTest )

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

  SET(TRACKER_FREQUENCY 30)

  ADD_TEST( igstkPolarisTrackerToolObserverToSocketRelayTest
    ${IGSTKSandbox_TESTS}
    igstkPolarisTrackerToolObserverToSocketRelayTest
    localhost 16666 1000 ${IGSTKSandbox_TEST_POLARIS_PORT_NUMBER}
    ${TRACKER_FREQUENCY}
    )

  ADD_TEST( igstkAuroraTrackerToolObserverToSocketRelayTest
    ${IGSTKSandbox_TESTS}
    igstkAuroraTrackerToolObserverToSocketRelayTest
    localhost 16666 1000 ${IGSTKSandbox_TEST_POLARIS_PORT_NUMBER}
    ${TRACKER_FREQUENCY}
    )

  ADD_TEST( igstkVicraTrackerToolObserverToSocketRelayTest
    ${IGSTKSandbox_TESTS}
    igstkPolarisTrackerToolObserverToSocketRelayTest
    localhost 16666 1000
    )

  IF(IGSTKSandbox_USE_OpenIGTLink)
    ADD_TEST( igstkAuroraTrackerToolObserverToOpenIGTLinkRelayTest
      ${IGSTKSandbox_TESTS}
      igstkAuroraTrackerToolObserverToOpenIGTLinkRelayTest
      localhost 16666 1000 ${IGSTKSandbox_TEST_POLARIS_PORT_NUMBER}
      ${TRACKER_FREQUENCY}
      )
  ENDIF(IGSTKSandbox_USE_OpenIGTLink)

  IF(IGSTKSandbox_USE_MicronTracker)
    ADD_TEST( igstkMicronTrackerToolObserverToSocketRelayTest
      ${IGSTKSandbox_TESTS}
      igstkMicronTrackerToolObserverToSocketRelayTest
      localhost 16666 1000 
      ${IGSTK_DATA_ROOT}/Input/CalibrationFiles
      ${IGSTK_DATA_ROOT}/Input/MicronTracker.ini
      ${IGSTK_DATA_ROOT}/Input/Markers )
  ENDIF(IGSTKSandbox_USE_MicronTracker)

  IF(IGSTKSandbox_USE_MicronTracker)
    ADD_TEST( igstkMicronTrackerToolObserverToOpenIGTLinkRelayTest
      ${IGSTKSandbox_TESTS}
      igstkMicronTrackerToolObserverToOpenIGTLinkRelayTest
      localhost 16666 1000 
      ${IGSTK_DATA_ROOT}/Input/CalibrationFiles
      ${IGSTK_DATA_ROOT}/Input/MicronTracker.ini
      ${IGSTK_DATA_ROOT}/Input/Markers )
  ENDIF(IGSTKSandbox_USE_MicronTracker)

ENDIF(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS)

ADD_TEST( igstkPivotCalibrationAlgorithmTest ${IGSTKSandbox_TESTS} igstkPivotCalibrationAlgorithmTest)
ADD_TEST( igstkPivotCalibrationTest ${IGSTKSandbox_TESTS}
igstkPivotCalibrationTest ${IGSTK_DATA_ROOT})
ADD_TEST( igstkPrincipalAxisCalibrationTest ${IGSTKSandbox_TESTS}
igstkPrincipalAxisCalibrationTest)
ADD_TEST( igstkToolCalibrationTest ${IGSTKSandbox_TESTS} igstkToolCalibrationTest)

ADD_TEST( igstkGroupObjectTest ${IGSTKSandbox_TESTS} igstkGroupObjectTest)
ADD_TEST( igstkVesselObjectTest ${IGSTKSandbox_TESTS} igstkVesselObjectTest)

ADD_TEST( igstkVascularNetworkObjectTest ${IGSTKSandbox_TESTS} igstkVascularNetworkObjectTest)

IF(IGSTKSandbox_USE_Qt)
 ADD_TEST( igstkImageResliceObjectRepresentationQtTest
              ${IGSTKSandbox_TESTS}
              --compare
${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png                       
${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkImageResliceObjectRepresentationQtTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot.png)
 ADD_TEST( igstkImageResliceObjectRepresentationQtTest2
              ${IGSTKSandbox_TESTS}
              igstkImageResliceObjectRepresentationQtTest2
              ${IGSTK_DATA_ROOT}/Input/E000192 
         )

 ADD_TEST( igstkImageResliceObjectRepresentationQtTest3
              ${IGSTKSandbox_TESTS}
              --compare
${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png                       
${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot3.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkImageResliceObjectRepresentationQtTest3
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot3.png)
 
ENDIF(IGSTKSandbox_USE_Qt)

IF(IGSTKSandbox_USE_FLTK) 
ADD_TEST( igstkImageResliceObjectRepresentationFltkTest
           ${IGSTKSandbox_TESTS}
           --compare
           ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkImageResliceObjectRepresentationFltkTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot.png)

ADD_TEST( igstkImageResliceObjectRepresentationFltkTest2
           ${IGSTKSandbox_TESTS}
           --compare
           ${IGSTKSandbox_DATA_ROOT}/Baseline/igstkImageResliceScreenShot2.png
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot2.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkImageResliceObjectRepresentationFltkTest2
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot2.png)


  ADD_TEST( igstkImageResliceObjectRepresentationFltkTest3
      ${IGSTKSandbox_TESTS}
           --compare
           ${IGSTK_DATA_ROOT}/Baseline/igstkViewScreenShot.png
                        ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot3.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkImageResliceObjectRepresentationFltkTest3
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkImageResliceScreenShot3.png
         )

  ADD_TEST( igstkCrossHairSpatialObjectTest
              ${IGSTKSandbox_TESTS}
              --compare
              ${IGSTKSandbox_DATA_ROOT}/Baseline/igstkCrossHairSpatialObjectTest.png                       
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkCrossHairSpatialObjectTest.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkCrossHairSpatialObjectTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkCrossHairSpatialObjectTest.png)
 
  ADD_TEST( igstkToolProjectionSpatialObjectTest
              ${IGSTKSandbox_TESTS}
              --compare
              ${IGSTK_DATA_ROOT}/Baseline/igstkToolProjectionSpatialObjectTest.png                       
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkToolProjectionSpatialObjectTest.png
              --toleranceIntensity 10
              --toleranceRadius    5
              --toleranceNumberOfPixels 5
              igstkToolProjectionSpatialObjectTest
              ${IGSTK_DATA_ROOT}/Input/E000192 
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkToolProjectionSpatialObjectTest.png)
 
ENDIF(IGSTKSandbox_USE_FLTK)



ADD_TEST( igstkReslicerPlaneSpatialObjectTest ${IGSTKSandbox_TESTS} 
          igstkReslicerPlaneSpatialObjectTest
          ${IGSTK_DATA_ROOT}/Input/E000192
        )

ADD_TEST( igstkToolCalibrationReaderTest ${IGSTKSandbox_TESTS}
             igstkToolCalibrationReaderTest
             ${IGSTK_DATA_ROOT}/Input/PivotCalibrationTool.txt
             ${IGSTK_DATA_ROOT}/Input/PivotCalibrationToolCorruptedOnPupose.txt
             )
ADD_TEST( igstkPivotCalibrationReaderTest ${IGSTKSandbox_TESTS}
             igstkPivotCalibrationReaderTest
             ${IGSTK_DATA_ROOT}/Input/PivotCalibrationTool.txt
             ${IGSTK_DATA_ROOT}/Input/PivotCalibrationToolCorruptedOnPupose.txt
             )


IF(IGSTKSandbox_USE_FLTK)
# FIXME: When done debugging, register it inside of the test drivers.
#ADD_EXECUTABLE( igstkCTImageSpatialObjectReadingAndRepresentationTest4 igstkCTImageSpatialObjectReadingAndRepresentationTest4.cxx)
#TARGET_LINK_LIBRARIES( igstkCTImageSpatialObjectReadingAndRepresentationTest4 IGSTKSandbox )
#ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest4
#  ${CXX_TEST_PATH}/igstkCTImageSpatialObjectReadingAndRepresentationTest4
#  ${IGSTK_DATA_ROOT}/Input/E000192 )
# END OF FIXME

 SET(SandboxTests_SRCS
    ${SandboxTests_SRCS}
  igstkImageResliceObjectRepresentationFltkTest.cxx)
 SET(SandboxTests_SRCS
    ${SandboxTests_SRCS}
  igstkImageResliceObjectRepresentationFltkTest2.cxx)

 SET(SandboxTests_SRCS
    ${SandboxTests_SRCS}
  igstkImageResliceObjectRepresentationFltkTest3.cxx)

# SET(SandboxTests_SRCS
#    ${SandboxTests_SRCS}
#  igstkAscensionTrackerTest.cxx)

 IF(IGSTKSandbox_TEST_FLOCKOFBIRD_ATTACHED)
    ADD_TEST( igstkFlockOfBirdsTrackerTest2
              ${IGSTKSandbox_TESTS}
              igstkAscensionTrackerTest
              ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAscensionTestLoggerOutput.txt
              ${IGSTKSandbox_TEST_FLOCKOFBIRD_PORT_NUMBER}
              )
  ENDIF(IGSTKSandbox_TEST_FLOCKOFBIRD_ATTACHED)

  SET(SandboxTests_SRCS ${SandboxTests_SRCS} 
                igstkCrossHairSpatialObjectTest.cxx)

  SET(SandboxTests_SRCS ${SandboxTests_SRCS} 
                igstkToolProjectionSpatialObjectTest.cxx )
 
ENDIF(IGSTKSandbox_USE_FLTK)


IF(IGSTKSandbox_DATA_ROOT)
  SET(IGSTKSandbox_BASELINE_DIR   "${IGSTKSandbox_DATA_ROOT}/Baseline")
  IF (IGSTKSandbox_USE_FLTK)
#    ADD_TEST( igstkObliqueImageSpatialObjectRepresentationTest
#              ${IGSTKSandbox_TESTS}
#              igstkObliqueImageSpatialObjectRepresentationTest
#              ${IGSTKSandbox_DATA_ROOT}/E000192)
#    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
#        igstkObliqueImageSpatialObjectRepresentationTest.cxx)

    ADD_TEST( igstkVascularNetworkReaderTest ${IGSTKSandbox_TESTS} 
          --compare ${BASELINE}/igstkVascularNetworkReaderTest.png 
          ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkVascularNetworkReaderTest.png 
          --toleranceIntensity 10
          --toleranceRadius 5
          --toleranceNumberOfPixels 5
          igstkVascularNetworkReaderTest 
          ${IGSTK_DATA_ROOT}/Input/vessel.tre
          ${IGSTK_DATA_ROOT}/Input/vesselCorruptedOnPurpose.tre 
          ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkVascularNetworkReaderTest.png
          )        
    SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkVascularNetworkReaderTest.cxx)

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

             #ADD_TEST(igstkStateMachineExportTest ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
             #TARGET_LINK_LIBRARIES(igstkStateMachineExportTest IGSTKSandbox)

#######-----------------------------------------------------------------------
# Now running IGSTK tests with the latest main library
INCLUDE(${IGSTK_SOURCE_DIR}/Testing/IGSTKTestingMacros.cmake)
IGSTKTesting(igstkLatestTests
             IGSTKSandbox
             1 
             ${IGSTK_DATA_ROOT}
             ${IGSTKSandbox_BINARY_DIR}
             IGSTKSandbox_TEST_AURORA_ATTACHED
             ${IGSTKSandbox_TEST_AURORA_PORT_NUMBER}
             IGSTKSandbox_TEST_POLARIS_ATTACHED
             ${IGSTKSandbox_TEST_POLARIS_PORT_NUMBER}
             IGSTKSandbox_TEST_LOOPBACK_ATTACHED
             ${IGSTKSandbox_TEST_LOOPBACK_PORT_NUMBER}
             IGSTKSandbox_USE_FLTK
             IGSTKSandbox_USE_Qt
             IGSTKSandbox_USE_MicronTracker
             IGSTKSandbox_USE_KWSTYLE
             )

