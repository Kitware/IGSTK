#
# Examples that do not require GUI
#

SUBDIRS(
 TransformReaders
 TransformWriters
 TrackerConfiguration
 Tools
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
    )
ENDIF(IGSTK_USE_FLTK)

IF (IGSTK_USE_OpenIGTLink)
    SUBDIRS(  OpenIGTLinkTrackerBroadcasting )
    SUBDIRS(  OpenIGTLinkConfigurationReaders )
ENDIF (IGSTK_USE_OpenIGTLink)




