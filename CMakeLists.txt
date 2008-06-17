CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT( IGSTKSandbox )

SET (LIBRARY_OUTPUT_PATH ${IGSTKSandbox_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${IGSTKSandbox_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables." FORCE)

# Add the testing directories
OPTION(IGSTKSandbox_BUILD_TESTING "Test the project" ON)
IF(IGSTKSandbox_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF(IGSTKSandbox_BUILD_TESTING)

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

IF(NOT IGSTKSandbox_SETUP_DASHBOARD)

FIND_PATH(IGSTK_SOURCE_DIR igstkConfigure.h.in)
IF(NOT IGSTK_SOURCE_DIR)
  MESSAGE( FATAL_ERROR "To build the sandbox, IGSTK CVS source directory path is
required. Please verify configuration")
ENDIF(NOT IGSTK_SOURCE_DIR)



# Disable MSVC 8 warnings
IF(WIN32)
  OPTION(DISABLE_MSVC8_DEPRECATED_WARNINGS "Disable Visual Studio 8 deprecated warnings" OFF)
  MARK_AS_ADVANCED(FORCE DISABLE_MSVC8_DEPRECATED_WARNINGS)  
  IF(DISABLE_MSVC8_DEPRECATED_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
  ENDIF(DISABLE_MSVC8_DEPRECATED_WARNINGS)
ENDIF(WIN32)


# The symbol WIN32 is not always defined. Most commonly it is _WIN32
# we add here the definition for WIN32.
IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
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


#  Use the correct VTK version 
IF( ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} LESS 3.4)
   MESSAGE(FATAL_ERROR "You are using ITK ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}. Use ITK version 3.4 or later")
ENDIF(${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} LESS 3.4 )

# Use the correct ITK version
IF( ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION} LESS 5.0 )
     MESSAGE(FATAL_ERROR "You are using VTK ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}. Use VTK version 5.0 or later")
ENDIF(${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION} LESS 5.0 )

#
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

#
# FIND Qt
#
OPTION(IGSTKSandbox_USE_Qt "Use Qt (The Fast Light Toolkit) for GUI" OFF)
IF(IGSTKSandbox_USE_Qt)
 # Check if VTK was built with GUISupport on
 IF( NOT VTK_USE_GUISUPPORT )
  MESSAGE(FATAL_ERROR "To build IGSTK with Qt support, you will need to use a VTK
build with GUI support i.e VTK_USE_GUISUPPORT turned on in VTK configuration")
 ENDIF( NOT VTK_USE_GUISUPPORT )

 # Check if VTK was built with Qt on
 IF( NOT VTK_USE_QVTK )
  MESSAGE(FATAL_ERROR "To build IGSTK with Qt support, you will need to use a VTK
build with Qt support i.e VTK_USE_QVTK turned on in VTK configuration")
 ENDIF( NOT VTK_USE_QVTK )
 # use what QVTK built with
 SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
 SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
 SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
 SET(DESIRED_QT_VERSION ${VTK_DESIRED_QT_VERSION} CACHE STRING "")

 IF( DESIRED_QT_VERSION MATCHES 3 )
    MESSAGE( FATAL_ERROR " VTK built using Qt 3. Please use Qt4" )
 ENDIF( DESIRED_QT_VERSION MATCHES 3)

 FIND_PACKAGE(Qt4 REQUIRED)

 IF(QT_USE_FILE)
   INCLUDE(${QT_USE_FILE})
 ELSE(QT_USE_FILE)
   SET(QT_LIBRARIES   ${QT_QT_LIBRARY})
 ENDIF(QT_USE_FILE)

 INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR})
ENDIF(IGSTKSandbox_USE_Qt)

# Use vtkKWImage, this will enable igstkGenericImageReader/Writer classes
OPTION(IGSTKSandbox_USE_vtkKWImage "Use vtkKWImage for generic image reading and writing" ON)

IF(IGSTKSandbox_USE_vtkKWImage)
  INCLUDE_DIRECTORIES( ${IGSTKSandbox_SOURCE_DIR}/Utilities/vtkKWImage )
ENDIF(IGSTKSandbox_USE_vtkKWImage)

# Connect MicronTracker
OPTION(IGSTKSandbox_USE_MicronTracker "Connect MicronTracker" OFF)

IF(IGSTKSandbox_USE_MicronTracker)
  FIND_PATH(MTC_INCLUDE_DIRECTORY MTC.h
  "C:/Program Files/Claron Technology/MicronTracker/Dist"
  DOC "Directory where the MTC.h file is located")
  INCLUDE_DIRECTORIES( ${MTC_INCLUDE_DIRECTORY} )
  INCLUDE_DIRECTORIES( ${IGSTKSandbox_SOURCE_DIR}/Utilities/MicronTracker/src )

  FIND_LIBRARY( MTC_LIBRARY 
    NAMES MTC MTC.dll 
    PATHS "C:/Program Files/Claron Technology/MicronTracker/Dist"
    DOC "Full path to the MTC.dll file"
    )

  IF(MTC_INCLUDE_DIRECTORY)
   INCLUDE(${IGSTK_SOURCE_DIR}/CMake/ExtractMTCVersion.cmake)
    IF( ${MTCMajorVersion}.${MTCMinorVersion} LESS 2.6 )
      MESSAGE( FATAL_ERROR " Your MTC version is
  ${MTCMajorVersion}.${MTCMinorVersion}. Version 2.6 or later should be used")
    ENDIF( ${MTCMajorVersion}.${MTCMinorVersion} LESS 2.6 )
    
    GET_FILENAME_COMPONENT( MTC_LIBRARY_DIRS ${MTC_LIBRARY} PATH )
    LINK_DIRECTORIES( ${MTC_LIBRARY_DIRS} )
  ENDIF(MTC_INCLUDE_DIRECTORY)
ENDIF(IGSTKSandbox_USE_MicronTracker)

# Enable the option for building classes for sending transform data through sockets
OPTION(IGSTKSandbox_SEND_TRANSFORMS_TO_SOCKETS "Setup classes for sending Tracker data through sockets" OFF)

# Enable test for hardware. This includes human supervised tests
# for the Trackers
OPTION(IGSTKSandbox_BUILD_SUPERVISED_TRACKER_TESTING "Build human supervised tests for Trackers" OFF)

#
# Define the options that enables the use of the Coordinate Reference Systems
#
OPTION(IGSTK_USE_COORDINATE_REFERENCE_SYSTEM "Use the new coordinate reference system infrastructure" ON)

#
# Use OpenIGTLink protocol 
#
OPTION(IGSTKSandbox_USE_OpenIGTLink "Use the OpenIGTLink protocol" ON)


#-----------------------------------------------------------------------------
# check for header files that we need
INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
# for serial communication: termio.h or termios.h?
CHECK_INCLUDE_FILE("termios.h"       HAVE_TERMIOS_H)
CHECK_INCLUDE_FILE("termio.h"        HAVE_TERMIO_H)

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
    SUBDIRS(IGSTK/Examples)
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

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/igstkSandboxConfigure.h.in
               ${PROJECT_BINARY_DIR}/igstkSandboxConfigure.h
               @ONLY IMMEDIATE)

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
