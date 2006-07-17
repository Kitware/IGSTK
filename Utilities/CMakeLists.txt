# Option for using code in the Atamai directory
OPTION(IGSTK_USE_ATAMAI "Use the code from Atamai directory" OFF)
MARK_AS_ADVANCED(IGSTK_USE_ATAMAI)
SET( IGSTK_USE_ATAMAI_CONFIG 0 )
IF( IGSTK_USE_ATAMAI )
    SUBDIRS( Atamai )
    SET( IGSTK_USE_ATAMAI_CONFIG 1 )
ENDIF( IGSTK_USE_ATAMAI )

#-----------------------------------------------------------------------------
# IGSTK uses KWStyle for checking the coding style
OPTION(IGSTK_USE_KWSTYLE "Run KWStyle in order to check for violations of the coding standard." OFF)
MARK_AS_ADVANCED(IGSTK_USE_KWSTYLE)

IF(IGSTK_USE_KWSTYLE)
  FIND_PROGRAM(KWSTYLE_EXECUTABLE
  NAMES KWStyle 
  PATHS
  /usr/local/bin
  )
  MARK_AS_ADVANCED(KWSTYLE_EXECUTABLE)

  CONFIGURE_FILE(${IGSTKSandbox_SOURCE_DIR}/Utilities/KWStyle/IGSTK.kws.xml.in
               ${IGSTKSandbox_BINARY_DIR}/IGSTK.kws.xml)

  ADD_CUSTOM_COMMAND(
    OUTPUT ${IGSTKSandbox_BINARY_DIR}/KWStyleReport.txt
    COMMAND ${KWSTYLE_EXECUTABLE}
    ARGS    -xml ${IGSTKSandbox_BINARY_DIR}/IGSTK.kws.xml -o ${IGSTKSandbox_SOURCE_DIR}/Utilities/KWStyle/IGSTKOverwrite.txt -R -v -d ${IGSTKSandbox_SOURCE_DIR} 
    COMMENT "Coding Style Checker"
    )
  ADD_CUSTOM_TARGET(StyleCheck DEPENDS ${IGSTKSandbox_BINARY_DIR}/KWStyleReport.txt)
ENDIF(IGSTK_USE_KWSTYLE)
