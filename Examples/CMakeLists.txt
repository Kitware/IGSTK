#
# Examples that do not require GUI
#

SUBDIRS(
 TransformReaders
 TransformWriters
 TrackerConfiguration
 Tools
 TrackerDataLogger
 MultiTrackerLogger
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
 


