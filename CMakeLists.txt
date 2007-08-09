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
# Don't include the IGSTK_USE_FILE since the Sandbox build is completely independent 
# of the IGSTK build.  We really just need to know where the IGSTK source is
# and where the IGSTK dependencies are: VTK, ITK, FLTK
#  INCLUDE (${IGSTK_USE_FILE})
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
  OPTION(DISABLE_MSVC8_DEPRECATED_WARNINGS "Disable Visual Studio 8 deprecated warnings" OFF)
  MARK_AS_ADVANCED(FORCE DISABLE_MSVC8_DEPRECATED_WARNINGS)  
  IF(DISABLE_MSVC8_DEPRECATED_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
  ENDIF(DISABLE_MSVC8_DEPRECATED_WARNINGS)
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

SET( IGSTKSandbox_USE_FLTK_VAR 0 )
IF( IGSTKSandbox_USE_FLTK )
  SET( IGSTKSandbox_USE_FLTK_VAR 1 )
ENDIF( IGSTKSandbox_USE_FLTK )

SET( IGSTK_USE_FLTK_VAR 0 )
IF( IGSTK_USE_FLTK )
  SET( IGSTK_USE_FLTK_VAR 1 )
ENDIF( IGSTK_USE_FLTK )

IF( IGSTKSandbox_USE_FLTK_VAR EQUAL IGSTK_USE_FLTK_VAR )
ELSE( IGSTKSandbox_USE_FLTK_VAR EQUAL IGSTK_USE_FLTK_VAR )
  MESSAGE( WARNING " The CMake flag for using FLTK is not matching between IGSTK and IGSTKSandbox")
ENDIF( IGSTKSandbox_USE_FLTK_VAR EQUAL IGSTK_USE_FLTK_VAR )

#
# FIND Qt
#
OPTION(IGSTKSandbox_USE_Qt "Use Qt for GUI" OFF)
IF(IGSTKSandbox_USE_Qt)
 # use what QVTK built with
 SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
 SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
 SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
 SET(DESIRED_QT_VERSION ${VTK_DESIRED_QT_VERSION} CACHE STRING "")
 FIND_PACKAGE(Qt4 REQUIRED)

 IF(QT_USE_FILE)
   INCLUDE(${QT_USE_FILE})
 ELSE(QT_USE_FILE)
   SET(QT_LIBRARIES   ${QT_QT_LIBRARY})
 ENDIF(QT_USE_FILE)

 INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR})

ENDIF(IGSTKSandbox_USE_Qt)


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
INCLUDE_DIRECTORIES(
${ITK_DIR}/Utilities/zlib
${IGSTKSandbox_SOURCE_DIR}/IGSTK/Source
${IGSTKSandbox_BINARY_DIR}/SourceLatest
${IGSTKSandbox_SOURCE_DIR}/Source
)


#-----------------------------------------------------------------------------
# Help other projects use IGSTKSandbox.
SET(IGSTKSandbox_LIBRARY_PATH ${LIBRARY_OUTPUT_PATH})


# Copy the UseIGSTKSandbox.cmake file to the binary tree for backward compatability.
CONFIGURE_FILE(${IGSTKSandbox_SOURCE_DIR}/UseIGSTKSandbox.cmake.in
               ${IGSTKSandbox_BINARY_DIR}/UseIGSTKSandbox.cmake COPYONLY IMMEDIATE)

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${IGSTKSandbox_BINARY_DIR}/IGSTKSandboxBuildSettings.cmake)
INSTALL_FILES(/lib/InsightToolkit .cmake IGSTKSandboxBuildSettings)

# Save library dependencies.
EXPORT_LIBRARY_DEPENDENCIES(${IGSTKSandbox_BINARY_DIR}/IGSTKSandboxLibraryDepends.cmake)

# Create the IGSTKSandboxConfig.cmake file containing the IGSTKSandbox configuration.
INCLUDE (${IGSTKSandbox_SOURCE_DIR}/igstkGenerateIGSTKSandboxConfig.cmake)

# Install some files.
INSTALL_FILES(/include/InsightToolkit .h igstkConfigure)
INSTALL_FILES(/lib/InsightToolkit .cmake UseIGSTKSandbox IGSTKSandboxLibraryDepends)



#-----------------------------------------------------------------------------
# Configure the default IGSTKSandbox_DATA_ROOT for the location of IGSTKSandbox Data.
#FIND_PATH(IGSTKSandbox_DATA_ROOT IGSTKSandboxData.readme ${IGSTKSandbox_SOURCE_DIR}/Testing/Data $ENV{IGSTKSandbox_DATA_ROOT})
#MARK_AS_ADVANCED(IGSTKSandbox_DATA_ROOT)

ENDIF(NOT IGSTKSandbox_SETUP_DASHBOARD)
