
SUBDIRS(TrackerConfiguration)
SUBDIRS(ImagerConfiguration)
SUBDIRS(CalibrationRegistrationLoading)
SUBDIRS(Tools)
SUBDIRS(ConfigurationFileReaders)
SUBDIRS(OIGTLinkConfigurationReaders)

IF(IGSTKSandbox_USE_FLTK) 
  SUBDIRS(FltkGuiTrackerConfiguration)
ENDIF(IGSTKSandbox_USE_FLTK)
