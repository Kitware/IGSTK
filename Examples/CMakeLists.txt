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
  AscensionTracker  
  PolarisTracker
  )

IF( IGSTKSandbox_USE_MicronTracker )
  SUBDIRS(
    MicronTracker
   ) 
ENDIF( IGSTKSandbox_USE_MicronTracker)

IF (IGSTKSandbox_USE_FLTK)
  SUBDIRS(
    CoordinateSystems
    OneViewAndTrackingNewUsingFLTKWidget
    TwoViews
    FourViewsTrackingWithCT
    FourViews
   # Comment out DeckOfCardRobot until it is updated to use the latest reslicing
   # classes
   #DeckOfCardRobot
    View
    Logging
    Navigator
    BronchoscopyNavigator
    WorkingVolumeTester
    TrackingVolumeViewer
    ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Examples/HelloWorld
    )
  IF (IGSTKSandbox_USE_MicronTracker)
    SUBDIRS( OneViewAndTrackingNewUsingMicronTracker)
  ENDIF(IGSTKSandbox_USE_MicronTracker)
  IF ( WIN32 )
    SUBDIRS( OneViewAndTrackingWebCameraUsingFLTKWidget )
  ENDIF ( WIN32 )
ENDIF (IGSTKSandbox_USE_FLTK)

IF (IGSTKSandbox_USE_OpenIGTLink)
    #SUBDIRS( UltrasoundNavigator )
    #SUBDIRS( UltrasoundCalibrationWizard )
    SUBDIRS( TransformOIGTLinkBroadcast )
ENDIF (IGSTKSandbox_USE_OpenIGTLink)

IF (IGSTKSandbox_USE_unicap)
    INCLUDE_DIRECTORIES(
    /usr/local/include/unicap
    )
    #todo compile this app also when CompressedDV is enabled
    SUBDIRS( VideoFrameGrabberAndViewer )
    #SUBDIRS( VideoTemporalCalibrationWizard )
ENDIF (IGSTKSandbox_USE_unicap)

IF (IGSTKSandbox_USE_Qt)
  SUBDIRS( OneViewAndTrackingUsingQTWidget )
  SUBDIRS( PolarisTrackingUsingQTWidget )
  IF (IGSTKSandbox_USE_MicronTracker)
     SUBDIRS( OneViewAndTrackingNewUsingQTWidgetAndMicronTracker )
  ENDIF(IGSTKSandbox_USE_MicronTracker)
   IF (APPLE)
     SUBDIRS(VideograbbingAndTrackingUsingQTWidget)
   ENDIF(APPLE)
ENDIF (IGSTKSandbox_USE_Qt)
