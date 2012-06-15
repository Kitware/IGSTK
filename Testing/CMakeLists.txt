#
# When adding an option or a specific setting to the testing environment remember
# to add the corresponding unset cache command under the else() scope below. This is
# required so that all these flags don't appear in the Cmake UI if BUILD_TESTING is off.
#
if( BUILD_TESTING )

  option( IGSTK_TEST_AURORA_ATTACHED "Aurora is attached to computer" OFF )
  set( IGSTK_TEST_AURORA_PORT_NUMBER 0 CACHE STRING
       "Serial port number (0, 1, 2, 3)" )

  option( IGSTK_TEST_POLARIS_ATTACHED "Polaris is attached to computer" OFF )
  set( IGSTK_TEST_POLARIS_PORT_NUMBER 0 CACHE STRING
       "Serial port number (0, 1, 2, 3)" )

  option( IGSTK_TEST_LOOPBACK_ATTACHED "Serial Loopback is attached" OFF )
  set( IGSTK_TEST_LOOPBACK_PORT_NUMBER 1 CACHE STRING
       "Serial port number (0, 1, 2, 3)" )

  option( IGSTK_TEST_FLOCKOFBIRD_ATTACHED "Flock of birds is attached to computer" OFF )
  set( IGSTK_TEST_FLOCKOFBIRD_PORT_NUMBER 0 CACHE STRING
       "Serial port number (0, 1, 2, 3)" )

  option( IGSTK_TEST_MicronTracker_ATTACHED "MicronTracker is attached to computer" OFF )

  option( IGSTK_TEST_Ascension3DGTracker_ATTACHED "Ascension3DGTracker is attached to computer" OFF )

  option( IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED "Atracsys EasyTrack500 Tracker is attached to computer" OFF )

  option( IGSTK_TEST_NDICertusTracker_ATTACHED "NDICertusTracker is attached to computer" OFF )

  include_directories(
    ${IGSTK_SOURCE_DIR}
    ${IGSTK_BINARY_DIR}
    ${IGSTK_SOURCE_DIR}/Source
    ${IGSTK_BINARY_DIR}/Source
    ${IGSTK_SOURCE_DIR}/Testing
    ${IGSTK_BINARY_DIR}/Testing
  )

  #include the cmake file that contains the testing macro. Add new tests
  #in IGSTKTestingMacros file.
  include( ${IGSTK_SOURCE_DIR}/Testing/IGSTKTestingMacros.cmake )

  #invoke the macro with the appropriate arguments
  set( SANDBOX_BUILD 0 )
  IGSTKTesting( igstkTests
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
else()

  unset( IGSTK_TEST_AURORA_ATTACHED CACHE )
  unset( IGSTK_TEST_AURORA_PORT_NUMBER CACHE )

  unset( IGSTK_TEST_POLARIS_ATTACHED CACHE )
  unset( IGSTK_TEST_POLARIS_PORT_NUMBER CACHE )

  unset( IGSTK_TEST_LOOPBACK_ATTACHED CACHE )
  unset( IGSTK_TEST_LOOPBACK_PORT_NUMBER CACHE )

  unset( IGSTK_TEST_FLOCKOFBIRD_ATTACHED CACHE )
  unset( IGSTK_TEST_FLOCKOFBIRD_PORT_NUMBER CACHE )

  unset( IGSTK_TEST_MicronTracker_ATTACHED CACHE )

  unset( IGSTK_TEST_Ascension3DGTracker_ATTACHED CACHE )

  unset( IGSTK_TEST_AtracsysEasyTrack500Tracker_ATTACHED CACHE )

  unset( IGSTK_TEST_NDICertusTracker_ATTACHED CACHE )

endif()
