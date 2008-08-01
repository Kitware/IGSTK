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
  ApplicationsCommon
  )

IF (IGSTKSandbox_USE_FLTK)
  SUBDIRS(
    OneViewAndTrackingNewUsingFLTKWidget
#    PivotCalibrationFLTKWidgetNew    
    TwoViews
    FourViewsTrackingWithCT
    FourViews
# Comment the UltrasoundGuidedRFA example
# since it requires the FlockOfBirds tracker
# and it is not yet fully tested
#    UltrasoundGuidedRFA
    NeedleBiopsy
    NeedleBiopsyWithReslicing
    NeedleBiopsyUnimodality
    DeckOfCardRobot
    View
    Logging
    )
  IF (IGSTKSandbox_USE_MicronTracker) 
    SUBDIRS( OneViewAndTrackingNewUsingMicronTracker)
  ENDIF(IGSTKSandbox_USE_MicronTracker) 
  IF ( WIN32 )
    SUBDIRS( OneViewAndTrackingWebCameraUsingFLTKWidget )
  ENDIF ( WIN32 )
ENDIF (IGSTKSandbox_USE_FLTK)

IF (IGSTKSandbox_USE_Qt)
#  SUBDIRS( OneViewAndTrackingUsingQTWidget )
#    IF (IGSTKSandbox_USE_MicronTracker) 
#      SUBDIRS( OneViewAndTrackingNewUsingQTWidgetAndMicronTracker )
#    ENDIF(IGSTKSandbox_USE_MicronTracker) 
   IF (APPLE)
     SUBDIRS(VideograbbingAndTrackingUsingQTWidget)
   ENDIF(APPLE)
ENDIF (IGSTKSandbox_USE_Qt)

