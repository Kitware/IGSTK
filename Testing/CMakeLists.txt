#Options for devices attached to serial port for testing
OPTION(IGSTK_TEST_AURORA_ATTACHED "Aurora is attached to computer" OFF)
SET( IGSTK_TEST_AURORA_PORT_NUMBER 0 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
OPTION(IGSTK_TEST_POLARIS_ATTACHED "Polaris is attached to computer" OFF)
SET( IGSTK_TEST_POLARIS_PORT_NUMBER 0 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )
OPTION(IGSTK_TEST_LOOPBACK_ATTACHED "Serial Loopback is attached" OFF)
SET( IGSTK_TEST_LOOPBACK_PORT_NUMBER 1 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )

OPTION(IGSTK_TEST_FLOCKOFBIRD_ATTACHED "Flock of birds is attached to computer" OFF)
SET( IGSTK_TEST_FLOCKOFBIRD_PORT_NUMBER 0 CACHE STRING
     "Serial port number (0, 1, 2, 3)" )

OPTION(IGSTK_TEST_MicronTracker_ATTACHED "MicronTracker is attached to computer" OFF)
OPTION(IGSTK_TEST_Ascension3DGTracker_ATTACHED "Ascension3DGTracker is attached to computer" OFF)
OPTION(IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED "Atracsys EasyTrack500 Tracker is attached to computer" OFF)
OPTION(IGSTK_TEST_NDICertusTracker_ATTACHED "NDICertusTracker is attached to computer" OFF)

INCLUDE_DIRECTORIES (
  ${IGSTK_SOURCE_DIR}
  ${IGSTK_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Source
  ${IGSTK_BINARY_DIR}/Source
  ${IGSTK_SOURCE_DIR}/Testing
  ${IGSTK_BINARY_DIR}/Testing
  )

#Include the cmake file that contains the testing macro. Add new tests
#in IGSTKTestingMacros file.
INCLUDE(${IGSTK_SOURCE_DIR}/Testing/IGSTKTestingMacros.cmake)

#invoke the macro with the appropriatae arguments 
SET(SANDBOX_BUILD 0)
IGSTKTesting(igstkTests
             IGSTK
             ${SANDBOX_BUILD}
             ${IGSTK_DATA_ROOT}
             ${IGSTK_BINARY_DIR}
             IGSTK_TEST_AURORA_ATTACHED
             ${IGSTK_TEST_AURORA_PORT_NUMBER}
             IGSTK_TEST_POLARIS_ATTACHED
             ${IGSTK_TEST_POLARIS_PORT_NUMBER}
             IGSTK_TEST_LOOPBACK_ATTACHED
             ${IGSTK_TEST_LOOPBACK_PORT_NUMBER}
             IGSTK_TEST_MicronTracker_ATTACHED
             IGSTK_USE_FLTK
             IGSTK_USE_Qt
             IGSTK_USE_MicronTracker
             IGSTK_USE_KWSTYLE
             IGSTK_TEST_FLOCKOFBIRD_ATTACHED
             ${IGSTK_TEST_FLOCKOFBIRD_PORT_NUMBER}
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

