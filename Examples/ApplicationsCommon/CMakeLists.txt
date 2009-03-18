
SUBDIRS(TrackerConfiguration)
SUBDIRS(ImagerConfiguration)

IF(IGSTKSandbox_USE_FLTK) 
  SUBDIRS(FltkGuiTrackerConfiguration)
ENDIF(IGSTKSandbox_USE_FLTK)
