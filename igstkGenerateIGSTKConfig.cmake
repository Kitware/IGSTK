# Generate the IGSTKConfig.cmake file in the build tree.  Also configure
# one for installation.  The file tells external projects how to use
# IGSTK.


#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The "use" file.
SET(IGSTK_USE_FILE ${IGSTK_BINARY_DIR}/UseIGSTK.cmake)

# The library dependencies file.
SET(IGSTK_LIBRARY_DEPENDS_FILE ${IGSTK_BINARY_DIR}/IGSTKLibraryDepends.cmake)

# The build settings file.
SET(IGSTK_BUILD_SETTINGS_FILE ${IGSTK_BINARY_DIR}/IGSTKBuildSettings.cmake)

# Library directory.
SET(IGSTK_LIBRARY_DIRS_CONFIG ${IGSTK_LIBRARY_PATH})

# Determine the include directories needed.
SET(IGSTK_INCLUDE_DIRS_CONFIG
  ${IGSTK_INCLUDE_DIRS_BUILD_TREE}
  ${IGSTK_INCLUDE_DIRS_SOURCE_TREE}
  ${IGSTK_INCLUDE_DIRS_SYSTEM}
)

#-----------------------------------------------------------------------------
# Configure IGSTKConfig.cmake for the build tree.
CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/IGSTKConfig.cmake.in
               ${IGSTK_BINARY_DIR}/IGSTKConfig.cmake @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# The "use" file.
SET(IGSTK_USE_FILE ${CMAKE_INSTALL_PREFIX}/lib/IgstkToolkit/UseIGSTK.cmake)

# The library dependencies file.
SET(IGSTK_LIBRARY_DEPENDS_FILE
    ${CMAKE_INSTALL_PREFIX}/lib/IgstkToolkit/IGSTKLibraryDepends.cmake)

# The build settings file.
SET(IGSTK_BUILD_SETTINGS_FILE
    ${CMAKE_INSTALL_PREFIX}/lib/IgstkToolkit/IGSTKBuildSettings.cmake)

# Include directories.
SET(IGSTK_INCLUDE_DIRS_CONFIG
  ${IGSTK_INCLUDE_DIRS_INSTALL_TREE}
  ${IGSTK_INCLUDE_DIRS_SYSTEM}
)

# Link directories.
SET(IGSTK_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/lib/IgstkToolkit)

#-----------------------------------------------------------------------------
# Configure IGSTKConfig.cmake for the install tree.
CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/IGSTKConfig.cmake.in
               ${IGSTK_BINARY_DIR}/Utilities/IGSTKConfig.cmake @ONLY IMMEDIATE)
