PROJECT( IGSTKSandbox )

SET (LIBRARY_OUTPUT_PATH ${IGSTKSandbox_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${IGSTKSandbox_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables." FORCE)

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/igstkSandboxConfigure.h.in
               ${PROJECT_BINARY_DIR}/igstkSandboxConfigure.h
               @ONLY IMMEDIATE)

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
  #INCLUDE (${IGSTK_USE_FILE})
ELSE (IGSTK_USE_FILE)
  MESSAGE( FATAL_ERROR "This application requires IGSTK. One of these components is missing. Please verify configuration")
ENDIF (IGSTK_USE_FILE)

# If we have IGSTK in the path we load its cache
IF(IGSTK_DIR)
  LOAD_CACHE(${IGSTK_DIR}
  EXCLUDE
    BUILD_TESTING
    BUILD_EXAMPLES
    LIBRARY_OUTPUT_PATH
    EXECUTABLE_OUTPUT_PATH
    MAKECOMMAND 
    SITE
    BUILDNAME
    CVS_UPDATE_OPTIONS
    DART_TESTING_TIMEOUT
    PURIFYCOMMAND
    CVSCOMMAND
    CMAKE_BACKWARDS_COMPATIBILITY
  )
ENDIF(IGSTK_DIR)

# Disable MSVC 8 warnings
IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(WIN32)

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
OPTION(IGSTKSandbox_USE_FLTK "Use FLTK (The Fast Light Toolkit) for GUI" ON)
IF(IGSTKSandbox_USE_FLTK)
 FIND_PACKAGE(FLTK)
 IF (FLTK_FOUND)
  INCLUDE_DIRECTORIES (${FLTK_INCLUDE_DIR})
ELSE (FLTK_FOUND)
  MESSAGE( FATAL_ERROR "This application requires FLTK. One of these components is missing. Please verify configuration")
ENDIF (FLTK_FOUND)
ENDIF(IGSTKSandbox_USE_FLTK)

# The IGSTKSandbox Library and any Utilities that it requires
SUBDIRS(Utilities Source)

# Add the testing directories
IF(IGSTKSandbox_BUILD_TESTING)
   SUBDIRS(Testing)
ENDIF(IGSTKSandbox_BUILD_TESTING)

# Add the examples directories
OPTION(IGSTKSandbox_BUILD_EXAMPLES "Examples of the project" ON)
IF(IGSTKSandbox_BUILD_EXAMPLES)
    SUBDIRS(Examples)
ENDIF(IGSTKSandbox_BUILD_EXAMPLES)

# include Sandbox header files before the main library IGSTK
INCLUDE_DIRECTORIES(BEFORE
${IGSTKSandbox_BINARY_DIR}/SourceLatest
)

#-----------------------------------------------------------------------------
# Configure the default IGSTKSandbox_DATA_ROOT for the location of IGSTKSandbox Data.
#FIND_PATH(IGSTKSandbox_DATA_ROOT IGSTKSandboxData.readme ${IGSTKSandbox_SOURCE_DIR}/Testing/Data $ENV{IGSTKSandbox_DATA_ROOT})
#MARK_AS_ADVANCED(IGSTKSandbox_DATA_ROOT)

ENDIF(NOT IGSTKSandbox_SETUP_DASHBOARD)
