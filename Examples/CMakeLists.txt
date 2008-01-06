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
    OneViewAndTrackingWebCameraUsingFLTKWidget
    PivotCalibrationFLTKWidget
    TwoViews
    FourViewsTrackingWithCT
    FourViews
    UltrasoundGuidedRFA
    NeedleBiopsy
    DeckOfCardRobot
    View
    Logging
    )
  IF (IGSTKSandbox_USE_MicronTracker) 
    SUBDIRS( OneViewAndTrackingUsingMicronTracker)
    SUBDIRS( OneViewAndTrackingNewUsingMicronTracker)
  ENDIF(IGSTKSandbox_USE_MicronTracker) 
ENDIF (IGSTK_USE_FLTK)

IF (IGSTKSandbox_USE_Qt)
  SUBDIRS( OneViewAndTrackingUsingQTWidget )
    IF (IGSTKSandbox_USE_MicronTracker) 
      SUBDIRS( OneViewAndTrackingNewUsingQTWidgetAndMicronTracker )
    ENDIF(IGSTKSandbox_USE_MicronTracker) 
ENDIF (IGSTKSandbox_USE_Qt)

