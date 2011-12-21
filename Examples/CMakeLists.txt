#
# Examples that do not require GUI
#

SUBDIRS(
 TransformReaders
 TransformWriters
 TrackerConfiguration
 Tools
 TrackerDataLogger
)



#
# Examples that require FLTK
#
IF(IGSTK_USE_FLTK)
  SUBDIRS(
    HelloWorld
    OneViewAndTracking
    FourViewsAndTracking
    TrackingVolumeViewer 
    NeedleBiopsy
    Navigator
    PivotCalibrationFLTKWidget
   )

   IF(IGSTK_USE_MicronTracker)
     SUBDIRS(OneViewAndTrackingUsingMicronTracker)
   ENDIF(IGSTK_USE_MicronTracker)
ENDIF(IGSTK_USE_FLTK)

IF(IGSTK_USE_Qt)
   IF(IGSTK_USE_MicronTracker)
     SUBDIRS(OneViewAndTrackingUsingQTWidgetAndMicronTracker)
   ENDIF(IGSTK_USE_MicronTracker)
ENDIF(IGSTK_USE_Qt)

IF (IGSTK_USE_OpenIGTLink)
    SUBDIRS(  OpenIGTLinkTrackerBroadcasting )
    SUBDIRS(  OpenIGTLinkConfigurationReaders )
ENDIF (IGSTK_USE_OpenIGTLink)

IF ( IGSTKSandbox_USE_AtracsysEasyTrack500 )
  SUBDIRS( MultiTrackerLogger )
ELSEIF ( IGSTKSandbox_USE_AXIOS3D_CAMBARB2 )
  SUBDIRS( MultiTrackerLogger )
ELSEIF ( IGSTKSandbox_USE_POLARIS_CLASSIC )
  SUBDIRS( MultiTrackerLogger )
ENDIF()

IF(IGSTK_USE_VideoImager)
 IF( WIN32 )
   IF (IGSTK_USE_OpenCV)
      SUBDIRS( VideoFrameGrabberAndViewerWebcam )
   ENDIF (IGSTK_USE_OpenCV)
 ENDIF( WIN32 )

 IF(IGSTK_USE_OpenIGTLink)
  SUBDIRS( VideoFrameGrabberAndViewerOpenIGTLink )
 ENDIF(IGSTK_USE_OpenIGTLink)

ENDIF(IGSTK_USE_VideoImager)
 


