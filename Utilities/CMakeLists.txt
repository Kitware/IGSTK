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

  OPTION(KWSTYLE_USE_VIM_FORMAT "Set KWStyle to generate errors with a VIM-compatible format." OFF)
  OPTION(KWSTYLE_USE_MSVC_FORMAT "Set KWStyle to generate errors with a VisualStudio-compatible format." OFF)

  IF(KWSTYLE_USE_VIM_FORMAT)
    SET(KWSTYLE_ARGUMENTS -vim ${KWSTYLE_ARGUMENTS})
  ENDIF(KWSTYLE_USE_VIM_FORMAT)

  IF(KWSTYLE_USE_MSVC_FORMAT)
    SET(KWSTYLE_ARGUMENTS -msvc ${KWSTYLE_ARGUMENTS})
  ENDIF(KWSTYLE_USE_MSVC_FORMAT)

  CONFIGURE_FILE(${IGSTKSandbox_SOURCE_DIR}/Utilities/KWStyle/IGSTK.kws.xml.in
               ${IGSTKSandbox_BINARY_DIR}/IGSTK.kws.xml)
               
  CONFIGURE_FILE(${IGSTKSandbox_SOURCE_DIR}/Utilities/KWStyle/IGSTKFiles.txt.in
               ${IGSTKSandbox_BINARY_DIR}/IGSTKFiles.txt)


  ADD_CUSTOM_COMMAND(
    OUTPUT ${IGSTKSandbox_BINARY_DIR}/KWStyleReport.txt
    COMMAND ${KWSTYLE_EXECUTABLE}
    ARGS    -xml ${IGSTKSandbox_BINARY_DIR}/IGSTK.kws.xml -o ${IGSTKSandbox_SOURCE_DIR}/Utilities/KWStyle/IGSTKOverwrite.txt ${KWSTYLE_ARGUMENTS} -D ${IGSTKSandbox_BINARY_DIR}/IGSTKFiles.txt
    COMMENT "Coding Style Checker"
    )
  ADD_CUSTOM_TARGET(StyleCheck DEPENDS ${IGSTKSandbox_BINARY_DIR}/KWStyleReport.txt)
ENDIF(IGSTK_USE_KWSTYLE)

IF( IGSTKSandbox_USE_MicronTracker )
    SUBDIRS( MicronTracker )
ENDIF( IGSTKSandbox_USE_MicronTracker )






