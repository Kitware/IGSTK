#-----------------------------------------------------------------------------
# Include directories for other projects installed on the system.
SET(IGSTK_INCLUDE_DIRS_SYSTEM "")


#-----------------------------------------------------------------------------
# Include directories from the build tree.
SET(IGSTK_INCLUDE_DIRS_BUILD_TREE ${IGSTK_BINARY_DIR})

# These directories are always needed.
SET(IGSTK_INCLUDE_DIRS_BUILD_TREE ${IGSTK_INCLUDE_DIRS_BUILD_TREE}
  ${IGSTK_SOURCE_DIR}/Source
  ${IGSTK_BINARY_DIR}/Source
)

# Patended include directories added only if the user explicitly enabled the
# IGSTK_USE_PATENTED option. Users are responsible for getting a license from the
# patent holders in order to use any of those methods.
IF(IGSTK_USE_PATENTED)
  SET(IGSTK_INCLUDE_DIRS_BUILD_TREE ${IGSTK_INCLUDE_DIRS_BUILD_TREE}
    ${IGSTK_SOURCE_DIR}/Code/Patented
    )
ENDIF(IGSTK_USE_PATENTED)


#-----------------------------------------------------------------------------
# Include directories needed for .cxx files in IGSTK.  These include
# directories will NOT be available to user projects.
SET(IGSTK_INCLUDE_DIRS_BUILD_TREE_CXX)


#-----------------------------------------------------------------------------
# Include directories from the install tree.
SET(IGSTK_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/InsightToolkit")
SET(IGSTK_INCLUDE_DIRS_INSTALL_TREE ${IGSTK_INCLUDE_DIRS_INSTALL_TREE}
  ${IGSTK_INSTALL_INCLUDE_DIR}
  ${IGSTK_INSTALL_INCLUDE_DIR}/Source
)

# Patended include directories added only if the user explicitly enabled the
# IGSTK_USE_PATENTED option. Users are responsible for getting a license from the
# patent holders in order to use any of those methods.
IF(IGSTK_USE_PATENTED)
  SET(IGSTK_INCLUDE_DIRS_INSTALL_TREE ${IGSTK_INCLUDE_DIRS_INSTALL_TREE}
    ${IGSTK_INSTALL_INCLUDE_DIR}/Code/Patented
    )
ENDIF(IGSTK_USE_PATENTED)

