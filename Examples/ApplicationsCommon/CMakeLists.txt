
IF(IGSTKSandbox_USE_OpenIGTLink)
  SUBDIRS(TrackerConfiguration)
ENDIF(IGSTKSandbox_USE_OpenIGTLink)

IF(IGSTKSandbox_USE_FLTK) 
  SUBDIRS(FltkGuiTrackerConfiguration)
ENDIF(IGSTKSandbox_USE_FLTK)
