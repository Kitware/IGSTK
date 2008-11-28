
#IF(IGSTKSandbox_USE_OpenIGTLink)
#  SUBDIRS(TrackerConfiguration)
#ENDIF(IGSTKSandbox_USE_OpenIGTLink)

SUBDIRS(TrackerConfiguration)
SUBDIRS(ImagerConfiguration)

IF(IGSTKSandbox_USE_FLTK) 
  SUBDIRS(FltkGuiTrackerConfiguration)
ENDIF(IGSTKSandbox_USE_FLTK)
