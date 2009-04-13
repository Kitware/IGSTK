
SUBDIRS(TrackerConfiguration)
SUBDIRS(ImagerConfiguration)
SUBDIRS(CalibrationRegistrationLoading)
SUBDIRS(Tools)

IF(IGSTKSandbox_USE_FLTK) 
  SUBDIRS(FltkGuiTrackerConfiguration)
ENDIF(IGSTKSandbox_USE_FLTK)
