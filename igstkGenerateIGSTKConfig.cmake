# Generate the IGSTKConfig.cmake file in the build tree.  Also configure
# one for installation.  The file tells external projects how to use
# IGSTK.


#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The "use" file.
SET(IGSTK_USE_FILE
   ${IGSTK_BINARY_DIR}/UseIGSTK.cmake
)

# The library dependencies file.
SET(IGSTK_LIBRARY_DEPENDS_FILE
   ${IGSTK_BINARY_DIR}/IGSTKLibraryDepends.cmake
)

# The build settings file.
SET(IGSTK_BUILD_SETTINGS_FILE
   ${IGSTK_BINARY_DIR}/IGSTKBuildSettings.cmake
)

# Library directory.
SET(IGSTK_LIBRARY_DIRS_CONFIG 
   ${IGSTK_LIBRARY_PATH}
   ${ITK_LIBRARY_DIRS}
   ${VTK_LIBRARY_DIRS}
)

IF(IGSTK_USE_FLTK)
   SET(IGSTK_LIBRARY_DIRS_CONFIG
      ${IGSTK_LIBRARY_DIRS_CONFIG}
      ${FLTK_LIBRARY_DIRS}
   )
ENDIF(IGSTK_USE_FLTK)

IF(IGSTK_USE_MicronTracker)
   GET_FILENAME_COMPONENT(MTC_LIBRARY_DIR ${MTC_LIBRARY} PATH)
   SET(IGSTK_LIBRARY_DIRS_CONFIG
      ${IGSTK_LIBRARY_DIRS_CONFIG}
      ${MTC_LIBRARY_DIR}
   )
ENDIF(IGSTK_USE_MicronTracker)

# Determine the include directories needed.
SET(IGSTK_INCLUDE_DIRS_CONFIG
   ${IGSTK_INCLUDE_DIRS_BUILD_TREE}
   ${IGSTK_INCLUDE_DIRS_SOURCE_TREE}
   ${IGSTK_INCLUDE_DIRS_SYSTEM}
)

IF(IGSTK_USE_MicronTracker)
   SET(IGSTK_INCLUDE_DIRS_SOURCE_TREE
      ${IGSTK_INCLUDE_DIRS_SOURCE_TREE}
      ${MTC_INCLUDE_DIRECTORY}
   )
ENDIF(IGSTK_USE_MicronTracker)

#-----------------------------------------------------------------------------
# Configure IGSTKConfig.cmake for the build tree.
CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/IGSTKConfig.cmake.in
               ${IGSTK_BINARY_DIR}/IGSTKConfig.cmake @ONLY IMMEDIATE)


#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# The "use" file.
SET(IGSTK_USE_FILE
   ${IGSTK_INSTALL_PACKAGE_DIR}/UseIGSTK.cmake
)

# The library dependencies file.
SET(IGSTK_LIBRARY_DEPENDS_FILE
   ${IGSTK_INSTALL_PACKAGE_DIR}/IGSTKLibraryDepends.cmake
)

# The build settings file.
SET(IGSTK_BUILD_SETTINGS_FILE
   ${IGSTK_INSTALL_PACKAGE_DIR}/IGSTKBuildSettings.cmake
)

# Include directories.
SET(IGSTK_INCLUDE_DIRS_CONFIG
#   ${IGSTK_INCLUDE_DIRS_INSTALL_TREE}
#   ${IGSTK_INCLUDE_DIRS_SYSTEM}
   ${IGSTK_INSTALL_INCLUDE_DIR}
)

# Link directories.
SET(IGSTK_LIBRARY_DIRS_CONFIG 
   ${IGSTK_INSTALL_LIB_DIR}
   ${ITK_LIBRARY_DIRS}
   ${VTK_LIBRARY_DIRS}
)

#-----------------------------------------------------------------------------
# Configure IGSTKConfig.cmake for the install tree.
CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/IGSTKConfig.cmake.in
               ${IGSTK_BINARY_DIR}/Utilities/IGSTKConfig.cmake @ONLY IMMEDIATE)
