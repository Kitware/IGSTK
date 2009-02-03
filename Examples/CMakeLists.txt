#
# Examples that do not require GUI
#

SUBDIRS(
 TransformReaders
 TrackerConfiguration
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


