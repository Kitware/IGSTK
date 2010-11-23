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
  SpatialObjects
  ApplicationsCommon
  AscensionTracker
  PolarisTracker
  TrackerDataLogger
  InterventionalPlanSettings
  InterventionalPlanSettingsIO
  )

# ITK v4.0 uses gdcm 2.x, this piece of code will not compile with the new ITK
IF (ITK_PRE4_VERSION)  
SUBDIRS( PETDicomReader )
ENDIF (ITK_PRE4_VERSION)  
  
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
    Reslicing
    Logging
    Navigator
    BronchoscopyNavigator
    WorkingVolumeTester
    TrackingVolumeViewer
    ${IGSTKSandbox_SOURCE_DIR}/IGSTK/Examples/HelloWorld
    PivotCalibrationFLTKWidget
    PETCTNeedleBiopsy
    )
  IF (IGSTKSandbox_USE_MicronTracker)
    SUBDIRS( OneViewAndTrackingNewUsingMicronTracker)
    SUBDIRS( VideoFrameGrabberAndViewerMicron )
  ENDIF(IGSTKSandbox_USE_MicronTracker)

  IF ( WIN32 )
    SUBDIRS( OneViewAndTrackingWebCameraUsingFLTKWidget )
    IF (IGSTKSandbox_USE_OpenCV)
      SUBDIRS( VideoFrameGrabberAndViewerWebcam
               LaparoscopyDemo
             )
    ENDIF (IGSTKSandbox_USE_OpenCV)
  ENDIF ( WIN32 )

  IF (IGSTKSandbox_USE_CompressedDV)
    SUBDIRS( VideoFrameGrabberAndViewerCompressedDV )
  ENDIF (IGSTKSandbox_USE_CompressedDV)

  IF (IGSTKSandbox_USE_unicap)
    INCLUDE_DIRECTORIES(
    /usr/local/include/unicap
    )
    #SUBDIRS( VideoTemporalCalibrationWizard )
  ENDIF (IGSTKSandbox_USE_unicap)

ENDIF (IGSTKSandbox_USE_FLTK)

IF (IGSTKSandbox_USE_OpenIGTLink)
    #SUBDIRS( UltrasoundNavigator )
    #SUBDIRS( UltrasoundCalibrationWizard )
    SUBDIRS(  OIGTLinkTrackerBroadcasting )
    SUBDIRS( VideoFrameGrabberAndViewerOpenIGTLink )
ENDIF (IGSTKSandbox_USE_OpenIGTLink)



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
