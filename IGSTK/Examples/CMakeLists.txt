#
# Examples that do not require GUI
#




#
# Examples that require FLTK
#
IF(IGSTK_USE_FLTK)
  SUBDIRS(
    HelloWorld
    OneViewAndTracking
    FourViewsAndTracking
    )
ENDIF(IGSTK_USE_FLTK)


