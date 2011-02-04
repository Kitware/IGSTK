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
  ${IGSTK_SOURCE_DIR}/Testing
  ${IGSTK_BINARY_DIR}/Testing
  ${ITK_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
)

IF(IGSTK_USE_FLTK)
 SET(IGSTK_INCLUDE_DIRS_BUILD_TREE ${IGSTK_INCLUDE_DIRS_BUILD_TREE}
     ${FLTK_INCLUDE_DIRS})
ENDIF(IGSTK_USE_FLTK)

# Patended include directories added only if the user explicitly enabled the
# IGSTK_USE_PATENTED option. Users are responsible for getting a license from the
# patent holders in order to use any of those methods.
IF(IGSTK_USE_PATENTED)
  SET(IGSTK_INCLUDE_DIRS_BUILD_TREE ${IGSTK_INCLUDE_DIRS_BUILD_TREE}
    ${IGSTK_SOURCE_DIR}/Code/Patented
    )
ENDIF(IGSTK_USE_PATENTED)

IF(IGSTK_USE_MicronTracker)
  SET(IGSTK_INCLUDE_DIRS_SOURCE_TREE ${IGSTK_INCLUDE_DIRS_SOURCE_TREE}
   ${MTC_INCLUDE_DIRECTORY} ${IGSTK_SOURCE_DIR}/Utilities/MicronTracker/src) 
ENDIF(IGSTK_USE_MicronTracker)

IF(IGSTK_USE_SceneGraphVisualization)
   SET(IGSTK_INCLUDE_DIRS_SOURCE_TREE ${IGSTK_INCLUDE_DIRS_SOURCE_TREE}
  ${IGSTK_SOURCE_DIR}/Utilities/SceneGraphVisualization)
ENDIF(IGSTK_USE_SceneGraphVisualization)

IF(IGSTK_USE_Ascension3DGTracker)
  SET(IGSTK_INCLUDE_DIRS_SOURCE_TREE ${IGSTK_INCLUDE_DIRS_SOURCE_TREE}
   ${ATC_INCLUDE_DIRECTORY} ) 
ENDIF(IGSTK_USE_Ascension3DGTracker)

#-----------------------------------------------------------------------------
# Include directories needed for .cxx files in IGSTK.  These include
# directories will NOT be available to user projects.
SET(IGSTK_INCLUDE_DIRS_BUILD_TREE_CXX)


#-----------------------------------------------------------------------------
# Include directories from the install tree.
SET(IGSTK_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/IGSTK")
SET(IGSTK_INCLUDE_DIRS_INSTALL_TREE ${IGSTK_INCLUDE_DIRS_INSTALL_TREE}
  ${IGSTK_INSTALL_INCLUDE_DIR}
  ${IGSTK_INSTALL_INCLUDE_DIR}/Source
  ${ITK_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
)


