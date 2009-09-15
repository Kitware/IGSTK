# Generate the IGSTKSandboxConfig.cmake file in the build tree.  Also configure
# one for installation.  The file tells external projects how to use
# IGSTKSandbox.


#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The "use" file.
SET(IGSTKSandbox_USE_FILE ${IGSTKSandbox_BINARY_DIR}/UseIGSTKSandbox.cmake)

# The library dependencies file.
SET(IGSTKSandbox_LIBRARY_DEPENDS_FILE ${IGSTKSandbox_BINARY_DIR}/IGSTKSandboxLibraryDepends.cmake)

# The build settings file.
SET(IGSTKSandbox_BUILD_SETTINGS_FILE ${IGSTKSandbox_BINARY_DIR}/IGSTKSandboxBuildSettings.cmake)

# Library directory.
SET(IGSTKSandbox_LIBRARY_DIRS_CONFIG ${IGSTKSandbox_LIBRARY_PATH})

IF(IGSTKSandbox_USE_MicronTracker)
   GET_FILENAME_COMPONENT(MTC_LIBRARY_DIR ${MTC_LIBRARY} PATH)
   SET(IGSTKSandbox_LIBRARY_DIRS_CONFIG
      ${IGSTKSandbox_LIBRARY_DIRS_CONFIG}
      ${MTC_LIBRARY_DIR}
   )
ENDIF(IGSTKSandbox_USE_MicronTracker)

# Determine the include directories needed.
SET(IGSTKSandbox_INCLUDE_DIRS_CONFIG
  ${IGSTKSandbox_INCLUDE_DIRS_BUILD_TREE}
  ${IGSTKSandbox_INCLUDE_DIRS_SOURCE_TREE}
  ${IGSTKSandbox_INCLUDE_DIRS_SYSTEM}
)

IF(IGSTKSandbox_USE_MicronTracker)
   SET(IGSTKSandbox_INCLUDE_DIRS_SOURCE_TREE
      ${IGSTKSandbox_INCLUDE_DIRS_SOURCE_TREE}
      ${MTC_INCLUDE_DIRECTORY}
   )
ENDIF(IGSTKSandbox_USE_MicronTracker)

#-----------------------------------------------------------------------------
# Configure IGSTKSandboxConfig.cmake for the build tree.
CONFIGURE_FILE(${IGSTKSandbox_SOURCE_DIR}/IGSTKSandboxConfig.cmake.in
               ${IGSTKSandbox_BINARY_DIR}/IGSTKSandboxConfig.cmake @ONLY)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# The "use" file.
SET(IGSTKSandbox_USE_FILE ${CMAKE_INSTALL_PREFIX}/lib/IgstkToolkit/UseIGSTKSandbox.cmake)

# The library dependencies file.
SET(IGSTKSandbox_LIBRARY_DEPENDS_FILE
    ${CMAKE_INSTALL_PREFIX}/lib/IgstkToolkit/IGSTKSandboxLibraryDepends.cmake)

# The build settings file.
SET(IGSTKSandbox_BUILD_SETTINGS_FILE
    ${CMAKE_INSTALL_PREFIX}/lib/IgstkToolkit/IGSTKSandboxBuildSettings.cmake)

# Include directories.
SET(IGSTKSandbox_INCLUDE_DIRS_CONFIG
  ${IGSTKSandbox_INCLUDE_DIRS_INSTALL_TREE}
  ${IGSTKSandbox_INCLUDE_DIRS_SYSTEM}
)

# Link directories.
SET(IGSTKSandbox_LIBRARY_DIRS_CONFIG ${CMAKE_INSTALL_PREFIX}/lib/IgstkToolkit)

#-----------------------------------------------------------------------------
# Configure IGSTKSandboxConfig.cmake for the install tree.
CONFIGURE_FILE(${IGSTKSandbox_SOURCE_DIR}/IGSTKSandboxConfig.cmake.in
               ${IGSTKSandbox_BINARY_DIR}/Utilities/IGSTKSandboxConfig.cmake @ONLY IMMEDIATE)
