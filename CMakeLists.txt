PROJECT( IGSTKSandbox )

SET (LIBRARY_OUTPUT_PATH ${IGSTKSandbox_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${IGSTKSandbox_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all executables." FORCE)

# Add the testing directories
OPTION(IGSTKSandbox_BUILD_TESTING "Test the project" ON)
IF(IGSTKSandbox_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF(IGSTKSandbox_BUILD_TESTING)

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

IF(NOT IGSTKSandbox_SETUP_DASHBOARD)


#
#  FIND IGSTK
#
FIND_PACKAGE(IGSTK)
IF (IGSTK_USE_FILE)
  INCLUDE (${IGSTK_USE_FILE})
ELSE (IGSTK_USE_FILE)
  MESSAGE( FATAL_ERROR "This application requires IGSTK. One of these components is missing. Please verify configuration")
ENDIF (IGSTK_USE_FILE)


#
#  FIND ITK
#
FIND_PACKAGE(ITK)
IF (USE_ITK_FILE)
  INCLUDE (${USE_ITK_FILE})
ELSE (USE_ITK_FILE)
  MESSAGE( FATAL_ERROR "This application requires ITK. One of these components is missing. Please verify configuration")
ENDIF (USE_ITK_FILE)


#
# FIND VTK
#
FIND_PACKAGE(VTK)
IF (USE_VTK_FILE)
  INCLUDE (${USE_VTK_FILE})
ELSE (USE_VTK_FILE)
  MESSAGE( FATAL_ERROR "This application requires VTK. One of these components is missing. Please verify configuration")
ENDIF (USE_VTK_FILE)



#
# FIND FLTK
#
FIND_PACKAGE(FLTK)
IF (FLTK_FOUND)
  INCLUDE_DIRECTORIES (${FLTK_INCLUDE_DIR})
ELSE (FLTK_FOUND)
  MESSAGE( FATAL_ERROR "This application requires FLTK. One of these components is missing. Please verify configuration")
ENDIF (FLTK_FOUND)

# The IGSTKSandbox Library
SUBDIRS(Source)

# Add the testing directories
IF(IGSTKSandbox_BUILD_TESTING)
   SUBDIRS(Testing)
ENDIF(IGSTKSandbox_BUILD_TESTING)

# Add the examples directories
OPTION(IGSTKSandbox_BUILD_EXAMPLES "Examples of the project" ON)
IF(IGSTKSandbox_BUILD_EXAMPLES)
    SUBDIRS(Examples)
ENDIF(IGSTKSandbox_BUILD_EXAMPLES)

# Option for using code in the Utilities directory
OPTION(IGSTK_USE_UTILITIES "Use the code in the Utilities directory" OFF)
MARK_AS_ADVANCED(IGSTK_USE_UTILITIES)
IF( IGSTK_USE_UTILITIES )
    SUBDIRS( Utilities )
ENDIF( IGSTK_USE_UTILITIES )


#-----------------------------------------------------------------------------
# Configure the default IGSTKSandbox_DATA_ROOT for the location of IGSTKSandbox Data.
#FIND_PATH(IGSTKSandbox_DATA_ROOT IGSTKSandboxData.readme ${IGSTKSandbox_SOURCE_DIR}/Testing/Data $ENV{IGSTKSandbox_DATA_ROOT})
#MARK_AS_ADVANCED(IGSTKSandbox_DATA_ROOT)

INCLUDE_DIRECTORIES(
${IGSTKSandbox_SOURCE_DIR}/Source
)

ENDIF(NOT IGSTKSandbox_SETUP_DASHBOARD)
