PROJECT( IGSTK )

# version
SET(IGSTK_VERSION_MAJOR "0")
SET(IGSTK_VERSION_MINOR "0")
SET(IGSTK_VERSION_PATCH "0")

SET (LIBRARY_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables." FORCE)

# Add the testing directories
OPTION(IGSTK_BUILD_TESTING "Test the project" ON)
IF(IGSTK_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF(IGSTK_BUILD_TESTING)

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

IF(NOT IGSTK_SETUP_DASHBOARD)

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
OPTION(IGSTK_USE_FLTK "Use FLTK (The Fast Light Toolkit) for GUI" ON)
IF(IGSTK_USE_FLTK)
  FIND_PACKAGE(FLTK)
  IF (FLTK_FOUND)
    INCLUDE_DIRECTORIES (${FLTK_INCLUDE_DIR})
    IF(WIN32)
      ADD_DEFINITIONS(-DWIN32)
    ENDIF(WIN32)
    LINK_LIBRARIES( ${FLTK_LIBRARIES} ${OPENGL_LIBRARIES} )
    IF(CYGWIN)
      LINK_LIBRARIES( ole32 uuid comctl32 wsock32 supc++ -lm -lgdi32)
    ENDIF(CYGWIN)
  ELSE (FLTK_FOUND)
    MESSAGE( FATAL_ERROR "This application requires FLTK. One of these components is missing. Please verify configuration")
  ENDIF (FLTK_FOUND)
ENDIF(IGSTK_USE_FLTK)

# The IGSTK Library
SUBDIRS(Source)

# Add the testing directories
IF(IGSTK_BUILD_TESTING)
   SUBDIRS(Testing)
ENDIF(IGSTK_BUILD_TESTING)

# Add the examples directories
OPTION(IGSTK_BUILD_EXAMPLES "Examples of the project" ON)
IF(IGSTK_BUILD_EXAMPLES)
  #  SUBDIRS(Examples)
ENDIF(IGSTK_BUILD_EXAMPLES)

#-----------------------------------------------------------------------------
# Configure the default IGSTK_DATA_ROOT for the location of IGSTK Data.
FIND_PATH(IGSTK_DATA_ROOT igstkDataReadMe.txt ${IGSTK_SOURCE_DIR}/Testing/Data $ENV{IGSTK_DATA_ROOT})
MARK_AS_ADVANCED(IGSTK_DATA_ROOT)

#-----------------------------------------------------------------------------
# The entire IGSTK tree should use the same include path, except for the
# Wrapping directory.

# Create the list of include directories needed for IGSTK header files.
INCLUDE(${IGSTK_SOURCE_DIR}/igstkIncludeDirectories.cmake)

# This should be the only INCLUDE_DIRECTORIES command in the entire
# tree, except for the Utilities and Wrapping directories.  We need to
# do this in one place to make sure the order is correct.
INCLUDE_DIRECTORIES(
  ${IGSTK_INCLUDE_DIRS_BUILD_TREE}
  ${IGSTK_INCLUDE_DIRS_BUILD_TREE_CXX}
  ${IGSTK_INCLUDE_DIRS_SYSTEM}
)

#-----------------------------------------------------------------------------
# check for header files that we need
INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
# for serial communication: termio.h or termios.h?
CHECK_INCLUDE_FILE("termios.h"       HAVE_TERMIOS_H)
CHECK_INCLUDE_FILE("termio.h"        HAVE_TERMIO_H)

# Configure a header needed by igstkSystemInformation.
CONFIGURE_FILE("${IGSTK_SOURCE_DIR}/igstkConfigure.h.in"
               "${IGSTK_BINARY_DIR}/igstkConfigure.h")

#-----------------------------------------------------------------------------
# define the right names for the serial devices according to operating system

IF(WIN32)
  SET(IGSTK_SERIAL_PORT_0 "COM1:" CACHE STRING "Device name for serial port 0")
  SET(IGSTK_SERIAL_PORT_1 "COM2:" CACHE STRING "Device name for serial port 1")
  SET(IGSTK_SERIAL_PORT_2 "COM3:" CACHE STRING "Device name for serial port 2")
  SET(IGSTK_SERIAL_PORT_3 "COM4:" CACHE STRING "Device name for serial port 3")
ELSE(WIN32)
  SET(IGSTK_SERIAL_PORT_0 "/dev/ttyS0" CACHE STRING
      "Device name for serial port 0")
  SET(IGSTK_SERIAL_PORT_1 "/dev/ttyS1" CACHE STRING
      "Device name for serial port 1")
  SET(IGSTK_SERIAL_PORT_2 "/dev/ttyUSB0" CACHE STRING
      "Device name for serial port 2")
  SET(IGSTK_SERIAL_PORT_3 "/dev/ttyUSB1" CACHE STRING
      "Device name for serial port 3")
ENDIF(WIN32)
MARK_AS_ADVANCED(IGSTK_SERIAL_PORT_0)
MARK_AS_ADVANCED(IGSTK_SERIAL_PORT_1)
MARK_AS_ADVANCED(IGSTK_SERIAL_PORT_2)
MARK_AS_ADVANCED(IGSTK_SERIAL_PORT_3)
#-----------------------------------------------------------------------------
# Help other projects use IGSTK.
SET(IGSTK_LIBRARY_PATH ${LIBRARY_OUTPUT_PATH})


# Copy the UseIGSTK.cmake file to the binary tree for backward compatability.
CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/UseIGSTK.cmake.in
               ${IGSTK_BINARY_DIR}/UseIGSTK.cmake COPYONLY IMMEDIATE)

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${IGSTK_BINARY_DIR}/IGSTKBuildSettings.cmake)
INSTALL_FILES(/lib/InsightToolkit .cmake IGSTKBuildSettings)

# Save library dependencies.
EXPORT_LIBRARY_DEPENDENCIES(${IGSTK_BINARY_DIR}/IGSTKLibraryDepends.cmake)

# Create the IGSTKConfig.cmake file containing the IGSTK configuration.
INCLUDE (${IGSTK_SOURCE_DIR}/igstkGenerateIGSTKConfig.cmake)

# Install some files.
INSTALL_FILES(/include/InsightToolkit .h igstkConfigure)
INSTALL_FILES(/lib/InsightToolkit .cmake UseIGSTK IGSTKLibraryDepends)

#-----------------------------------------------------------------------------
# Add compiler flags IGSTK needs to work on this platform.  This must be
# done AFTER the call to CMAKE_EXPORT_BUILD_SETTINGS.

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IGSTK_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGSTK_REQUIRED_CXX_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${IGSTK_REQUIRED_LINK_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${IGSTK_REQUIRED_LINK_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${IGSTK_REQUIRED_LINK_FLAGS}")


ENDIF(NOT IGSTK_SETUP_DASHBOARD)

OPTION(BUILD_DOCUMENTATION "Build IGSTK Documentation" OFF)
IF(BUILD_DOCUMENTATION)
  FIND_PACKAGE(VTK)
  IF (USE_VTK_FILE)
    INCLUDE (${USE_VTK_FILE})
  ELSE (USE_VTK_FILE)
    MESSAGE( FATAL_ERROR "This application requires VTK. One of these components is missing. Please verify configuration")
  ENDIF (USE_VTK_FILE)
  SUBDIRS(Utilities/Doxygen)
ENDIF(BUILD_DOCUMENTATION)
