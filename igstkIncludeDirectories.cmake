#-----------------------------------------------------------------------------
# Include directories for other projects installed on the system.
SET(IGSTKSandbox_INCLUDE_DIRS_SYSTEM "")


#-----------------------------------------------------------------------------
# Include directories from the build tree.
SET(IGSTKSandbox_INCLUDE_DIRS_BUILD_TREE ${IGSTKSandbox_BINARY_DIR})

# These directories are always needed.
SET(IGSTKSandbox_INCLUDE_DIRS_BUILD_TREE ${IGSTKSandbox_INCLUDE_DIRS_BUILD_TREE}
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/Source
)

# Patended include directories added only if the user explicitly enabled the
# IGSTKSandbox_USE_PATENTED option. Users are responsible for getting a license from the
# patent holders in order to use any of those methods.
IF(IGSTKSandbox_USE_PATENTED)
  SET(IGSTKSandbox_INCLUDE_DIRS_BUILD_TREE ${IGSTKSandbox_INCLUDE_DIRS_BUILD_TREE}
    ${IGSTKSandbox_SOURCE_DIR}/Code/Patented
    )
ENDIF(IGSTKSandbox_USE_PATENTED)


#-----------------------------------------------------------------------------
# Include directories needed for .cxx files in IGSTKSandbox.  These include
# directories will NOT be available to user projects.
SET(IGSTKSandbox_INCLUDE_DIRS_BUILD_TREE_CXX)


#-----------------------------------------------------------------------------
# Include directories from the install tree.
SET(IGSTKSandbox_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/InsightToolkit")
SET(IGSTKSandbox_INCLUDE_DIRS_INSTALL_TREE ${IGSTKSandbox_INCLUDE_DIRS_INSTALL_TREE}
  ${IGSTKSandbox_INSTALL_INCLUDE_DIR}
  ${IGSTKSandbox_INSTALL_INCLUDE_DIR}/Source
)

# Patended include directories added only if the user explicitly enabled the
# IGSTKSandbox_USE_PATENTED option. Users are responsible for getting a license from the
# patent holders in order to use any of those methods.
IF(IGSTKSandbox_USE_PATENTED)
  SET(IGSTKSandbox_INCLUDE_DIRS_INSTALL_TREE ${IGSTKSandbox_INCLUDE_DIRS_INSTALL_TREE}
    ${IGSTKSandbox_INSTALL_INCLUDE_DIR}/Code/Patented
    )
ENDIF(IGSTKSandbox_USE_PATENTED)

