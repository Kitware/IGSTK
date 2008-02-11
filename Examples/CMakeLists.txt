INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_SOURCE_DIR}
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/Source
  )

SUBDIRS( 
  LandmarkRegistration
  LandmarkRegistrationErrorEstimation
  DICOMImageReader
  PivotCalibration
  PrincipalAxisCalibration
  PivotCalibrationReader
  SpatialObjects
  )

IF (IGSTK_USE_FLTK)
  SUBDIRS(
    OneViewAndTrackingUsingFLTKWidget
    OneViewAndTrackingNewUsingFLTKWidget
    PivotCalibrationFLTKWidget
    TwoViews
    FourViewsTrackingWithCT
    FourViews
# Comment the UltrasoundGuidedRFA example
# since it requires the FlockOfBirds tracker
# which is not yet fully tested
#    UltrasoundGuidedRFA
    NeedleBiopsy
    DeckOfCardRobot
    View
    Logging
    )
  IF (IGSTKSandbox_USE_MicronTracker) 
    SUBDIRS( OneViewAndTrackingUsingMicronTracker)
    SUBDIRS( OneViewAndTrackingNewUsingMicronTracker)
  ENDIF(IGSTKSandbox_USE_MicronTracker) 
  IF ( WIN32 )
    SUBDIRS( OneViewAndTrackingWebCameraUsingFLTKWidget )
  ENDIF ( WIN32 )
ENDIF (IGSTK_USE_FLTK)

IF (IGSTK_USE_Qt)
#  SUBDIRS( OneViewAndTrackingUsingQTWidget )
#    IF (IGSTK_USE_MicronTracker) 
#      SUBDIRS( OneViewAndTrackingNewUsingQTWidgetAndMicronTracker )
#    ENDIF(IGSTK_USE_MicronTracker) 
ENDIF (IGSTK_USE_Qt)

