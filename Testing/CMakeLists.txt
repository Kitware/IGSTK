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
OPTION(IGSTKSandbox_TEST_FLOCKOFBIRD_ATTACHED "FlockOfBird is attached" OFF)
SET( IGSTKSandbox_TEST_FLOCKOFBIRD_PORT_NUMBER 1 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
SET(IGSTK_DATA_ROOT ${IGSTK_SOURCE_DIR}/Testing/Data)

OPTION(IGSTKSandbox_TEST_Ascension3DG_ATTACHED "Ascension3DG is attached to computer" OFF)
OPTION(IGSTKSandbox_TEST_MicronTracker_ATTACHED "Ascension3DG is attached to computer" OFF)

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

  igstkGroupObjectTest.cxx
  igstkVesselObjectTest.cxx
  igstkDICOMGenericImageReaderTest.cxx
  igstkGenericImageSpatialObjectTest.cxx
  igstkLandmarkUltrasoundCalibrationTest.cxx
  igstkSocketCommunicationTest.cxx
  igstkVascularNetworkObjectTest.cxx

  igstkDefaultWidget.h
  igstkDefaultWidget.cxx


)

ADD_TEST( igstkDICOMGenericImageReaderTest ${IGSTKSandbox_TESTS} igstkDICOMGenericImageReaderTest
   ${IGSTK_DATA_ROOT}/Input/E000192 )

ADD_TEST( igstkGenericImageSpatialObjectTest ${IGSTKSandbox_TESTS} igstkGenericImageSpatialObjectTest)

ADD_TEST( igstkLandmarkUltrasoundCalibrationTest ${IGSTKSandbox_TESTS} igstkLandmarkUltrasoundCalibrationTest
  ${IGSTKSandbox_DATA_ROOT}/Input
  )

ADD_TEST( igstkSocketCommunicationTest ${IGSTKSandbox_TESTS} igstkSocketCommunicationTest)
ADD_TEST( igstkTransductionMacroTest ${IGSTKSandbox_TESTS} igstkTransductionMacroTest)

ADD_TEST( igstkGroupObjectTest ${IGSTKSandbox_TESTS} igstkGroupObjectTest)
ADD_TEST( igstkVesselObjectTest ${IGSTKSandbox_TESTS} igstkVesselObjectTest)

ADD_TEST( igstkVascularNetworkObjectTest ${IGSTKSandbox_TESTS} igstkVascularNetworkObjectTest)

IF(IGSTKSandbox_USE_FLTK) 
ENDIF(IGSTKSandbox_USE_FLTK)


ADD_TEST( igstkReslicerPlaneSpatialObjectTest ${IGSTKSandbox_TESTS} 
          igstkReslicerPlaneSpatialObjectTest
          ${IGSTK_DATA_ROOT}/Input/E000192
        )

IF(IGSTKSandbox_USE_FLTK)
# FIXME: When done debugging, register it inside of the test drivers.
#ADD_EXECUTABLE( igstkCTImageSpatialObjectReadingAndRepresentationTest4 igstkCTImageSpatialObjectReadingAndRepresentationTest4.cxx)
#TARGET_LINK_LIBRARIES( igstkCTImageSpatialObjectReadingAndRepresentationTest4 IGSTKSandbox )
#ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest4
#  ${CXX_TEST_PATH}/igstkCTImageSpatialObjectReadingAndRepresentationTest4
#  ${IGSTK_DATA_ROOT}/Input/E000192 )
# END OF FIXME

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

IF( ${IGSTKSandbox_TEST_Ascension3DG_ATTACHED} )
  ADD_TEST( igstkAscension3DGTrackerTest ${IGSTKSandbox_TESTS} igstkAscension3DGTrackerTest
            ${IGSTKSandbox_TEST_OUTPUT_DIR}/igstkAscension3DGTrackerTestLogOutput.txt  )
  SET(SandboxTests_SRCS ${SandboxTests_SRCS}
        igstkAscension3DGTrackerTest.cxx)
ENDIF (${IGSTKSandbox_TEST_Ascension3DG_ATTACHED})

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



# Configure testing data to used in the example applications

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/TrackerConfiguration/micronConfiguration.xml.in"
              "${CMAKE_CURRENT_BINARY_DIR}/Data/TrackerConfiguration/micronConfiguration.xml"
              @ONLY IMMEDIATE)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/TrackerConfiguration/ascensionConfiguration.xml.in"
              "${CMAKE_CURRENT_BINARY_DIR}/Data/TrackerConfiguration/ascensionConfiguration.xml"
              @ONLY IMMEDIATE)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/TrackerConfiguration/polarisHybridConfiguration.xml.in"
              "${CMAKE_CURRENT_BINARY_DIR}/Data/TrackerConfiguration/polarisHybridConfiguration.xml"
              @ONLY IMMEDIATE)

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/TrackerConfiguration/auroraConfiguration.xml.in"
              "${CMAKE_CURRENT_BINARY_DIR}/Data/TrackerConfiguration/auroraConfiguration.xml"
              @ONLY IMMEDIATE)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/Data/TrackerConfiguration/vicraConfiguration.xml.in"
              "${CMAKE_CURRENT_BINARY_DIR}/Data/TrackerConfiguration/vicraConfiguration.xml"
              @ONLY IMMEDIATE)


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
             IGSTKSandbox_TEST_MicronTracker_ATTACHED
             IGSTKSandbox_USE_FLTK
             IGSTKSandbox_USE_Qt
             IGSTKSandbox_USE_MicronTracker
             IGSTKSandbox_USE_KWSTYLE
             IGSTKSandbox_TEST_FLOCKOFBIRD_ATTACHED
             ${IGSTKSandbox_TEST_FLOCKOFBIRD_PORT_NUMBER}
             IGSTKSandbox_USE_OpenIGTLink
             IGSTKSandbox_USE_VideoImager
             IGSTKSandbox_USE_InfiniTrack
             )

