# Option for using code in the Atamai directory
OPTION(IGSTK_USE_ATAMI "Use the code from Atamai directory" OFF)
MARK_AS_ADVANCED(IGSTK_USE_ATAMI)
IF( IGSTK_USE_ATAMI )
    SUBDIRS( Atamai )
ENDIF( IGSTK_USE_ATAMI )


