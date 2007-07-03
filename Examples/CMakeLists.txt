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
    TwoViews
    FourViewsTrackingWithCT
    FourViews
    UltrasoundGuidedRFA
    NeedleBiopsy
    DeckOfCardRobot
    View
    Logging
    )

ENDIF (IGSTK_USE_FLTK)

