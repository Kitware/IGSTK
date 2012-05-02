CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT( IGSTK )

# version
SET(IGSTK_VERSION_MAJOR "4")
SET(IGSTK_VERSION_MINOR "4")
SET(IGSTK_VERSION_PATCH "0")

SET (LIBRARY_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables." FORCE)

# Append the library version information to the library target properties.
OPTION(IGSTK_WITH_LIBRARY_VERSION "Build with library version information" OFF)
IF(IGSTK_WITH_LIBRARY_VERSION)
  SET(IGSTK_LIBRARY_PROPERTIES ${IGSTK_LIBRARY_PROPERTIES}
#   BUILD_WITH_INSTALL_RPATH OFF
#   INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
#   INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib
    VERSION ${IGSTK_VERSION_MAJOR}.${IGSTK_VERSION_MINOR}.${IGSTK_VERSION_PATCH}
    SOVERSION ${IGSTK_VERSION_MAJOR}
  )
ENDIF(IGSTK_WITH_LIBRARY_VERSION)

# Build with shared libraries
OPTION(BUILD_SHARED_LIBS "Build with shared libraries" OFF)

# Add the testing directories
OPTION(IGSTK_BUILD_TESTING "Test the project" ON)

IF(IGSTK_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF(IGSTK_BUILD_TESTING)

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

# Determine if this is development or stable version
OPTION(IGSTK_DEVELOPMENT_VERSION "Set IGSTK version to development" ON)

#
#  FIND ITK
#
FIND_PACKAGE(ITK)
IF (USE_ITK_FILE)
  INCLUDE (${USE_ITK_FILE})
ELSE (USE_ITK_FILE)
  MESSAGE( FATAL_ERROR "This application requires ITK. One of these components is missing. Please verify configuration")
ENDIF (USE_ITK_FILE)

IF( ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} VERSION_LESS 4.0 )
  SET(ITK_PRE4_VERSION 1)
ELSE( ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} VERSION_LESS 4.0 )
  SET(ITK_PRE4_VERSION 0)
ENDIF( ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} VERSION_LESS 4.0 )
   
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
OPTION(IGSTK_USE_FLTK "Use FLTK (The Fast Light Toolkit) for GUI" OFF)
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

#
# FIND Qt
#
OPTION(IGSTK_USE_Qt "Use Qt for GUI" OFF)
IF(IGSTK_USE_Qt)
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

ENDIF(IGSTK_USE_Qt)

# The IGSTK Library
SUBDIRS(Source)

# Add the testing directories
IF(IGSTK_BUILD_TESTING)
   SUBDIRS(Testing)
ENDIF(IGSTK_BUILD_TESTING)

# Add the examples directories
OPTION(IGSTK_BUILD_EXAMPLES "Examples of the project" ON)
IF(IGSTK_BUILD_EXAMPLES)
   SUBDIRS(Examples)
ENDIF(IGSTK_BUILD_EXAMPLES)

# Connect MicronTracker
OPTION(IGSTK_USE_MicronTracker "Connect MicronTracker" OFF)

IF(IGSTK_USE_MicronTracker)
  FIND_PATH(MTC_INCLUDE_DIRECTORY MTC.h
  "C:/Program Files/Claron Technology/MicronTracker/Dist"
  DOC "Directory where the MTC.h file is located")
  INCLUDE_DIRECTORIES( ${MTC_INCLUDE_DIRECTORY} )
  INCLUDE_DIRECTORIES( ${IGSTK_SOURCE_DIR}/Utilities/MicronTracker/src )

  FIND_LIBRARY( MTC_LIBRARY
    NAMES MTC MTC.dll
    PATHS "C:/Program Files/Claron Technology/MicronTracker/Dist"
    DOC "Full path to the MTC.dll file"
    )

  IF(MTC_INCLUDE_DIRECTORY)
   INCLUDE(${IGSTK_SOURCE_DIR}/CMake/ExtractMTCVersion.cmake)
    IF( ${MTCMajorVersion}.${MTCMinorVersion} LESS 3.6.1 )
      MESSAGE( FATAL_ERROR " Your MTC version is
  ${MTCMajorVersion}.${MTCMinorVersion}. Version 3.6.1 or later should be used")
    ENDIF( ${MTCMajorVersion}.${MTCMinorVersion} LESS 3.6.1 )

    GET_FILENAME_COMPONENT( MTC_LIBRARY_DIRS ${MTC_LIBRARY} PATH )
    LINK_DIRECTORIES( ${MTC_LIBRARY_DIRS} )
  ENDIF(MTC_INCLUDE_DIRECTORY)
ENDIF(IGSTK_USE_MicronTracker)
# Connect InfiniTrack
OPTION(IGSTK_USE_InfiniTrack "Connect InfiniTrack " OFF)

IF(IGSTK_USE_InfiniTrack)
  IF(WIN32)
    FIND_PATH(InfiniTrack_INCLUDE_DIRECTORY itkInterface.h
    "C:/Program Files/Atracsys/infiniTrack/include"
    DOC "Directory where the itkInterface.h file is located")
    INCLUDE_DIRECTORIES( ${InfiniTrack_INCLUDE_DIRECTORY} )

    FIND_LIBRARY( InfiniTrack_LIBRARY
      NAMES v9ITK.dll v9ITK
      PATHS "C:/Program Files/Atracsys/infiniTrack/lib"
      DOC "Full path to the InfiniTrack library file"
      )

    IF(InfiniTrack_INCLUDE_DIRECTORY)
      GET_FILENAME_COMPONENT( InfiniTrack_LIBRARY_DIRS ${InfiniTrack_LIBRARY} PATH )
      LINK_DIRECTORIES( ${InfiniTrack_LIBRARY_DIRS} )
    ENDIF(InfiniTrack_INCLUDE_DIRECTORY)

  ELSE(WIN32)
    MESSAGE(FATAL_ERROR " Please turn off InfiniTrack option. Only windows operating system is currently supported for InfiniTrack")
  ENDIF(WIN32)

ENDIF(IGSTK_USE_InfiniTrack)

# Connect Ascension 3DG tracker
OPTION(IGSTK_USE_Ascension3DGTracker "Connect Ascension 3D Guidance Tracker" OFF)
IF(IGSTK_USE_Ascension3DGTracker)
  OPTION(Ascension3DG_MedSafe "Use MedSafe version of the tracker" OFF)
  IF(Ascension3DG_MedSafe) # Use MedSafe version
    FIND_PATH(ATC_INCLUDE_DIRECTORY ATC3DGm.h
    "C:/Program Files/Ascension/3D Guidance medSAFE (Rev F)/3D Guidance API"
    DOC "Directory where the ATC3DGm.h file is located")
    INCLUDE_DIRECTORIES( ${ATC_INCLUDE_DIRECTORY} )

    FIND_LIBRARY( ATC_LIBRARY 
      NAMES ATC3DGm ATC3DGm.dll
      PATHS "C:/Program Files/Ascension/3D Guidance medSAFE (Rev F)/3D Guidance API"
      DOC "Full path to the ATC3DG.dll file"
      )
  ELSE(Ascension3DG_MedSafe) # Use non MedSafe version
    FIND_PATH(ATC_INCLUDE_DIRECTORY ATC3DG.h
    "C:/Program Files/Ascension/3D Guidance (Rev D)/3D Guidance API"
    DOC "Directory where the ATC3DG.h file is located")
    INCLUDE_DIRECTORIES( ${ATC_INCLUDE_DIRECTORY} )

    FIND_LIBRARY( ATC_LIBRARY 
      NAMES ATC3DG ATC3DG.dll
      PATHS "C:/Program Files/Ascension/3D Guidance (Rev D)/3D Guidance API"
      DOC "Full path to the ATC3DG.dll file"
      )
  ENDIF(Ascension3DG_MedSafe)
  
  GET_FILENAME_COMPONENT( ATC_LIBRARY_DIRS ${ATC_LIBRARY} PATH )
  LINK_DIRECTORIES( ${ATC_LIBRARY_DIRS} )
ENDIF(IGSTK_USE_Ascension3DGTracker)

#
# Use OpenIGTLink protocol
#
OPTION(IGSTK_USE_OpenIGTLink "Use the OpenIGTLink protocol" OFF)
IF(IGSTK_USE_OpenIGTLink)
  #
  #  FIND OpenIGTLink
  #
  FIND_PACKAGE(OpenIGTLink)
  IF (OpenIGTLink_USE_FILE)
    INCLUDE (${OpenIGTLink_USE_FILE})
  ELSE (OpenIGTLink_USE_FILE)
    MESSAGE( FATAL_ERROR "This application requires OpenIGTLink. One of these components is missing. Please verify configuration")
  ENDIF (OpenIGTLink_USE_FILE)
ENDIF(IGSTK_USE_OpenIGTLink)

#
# Use OpenCV
#
OPTION(IGSTK_USE_OpenCV "Use the OpenCV library" OFF)
IF(IGSTK_USE_OpenCV)
  #
  #  FIND OpenCV
  FIND_PACKAGE( OpenCV REQUIRED )
ENDIF(IGSTK_USE_OpenCV)


#
# This option enables the scene graph observer to collect relavant events to constuct the scene graph tree
#
OPTION(IGSTK_USE_SceneGraphVisualization "Enable scene graph visualization by allowing the observer to listen to relevant events" ON)
IF(IGSTK_USE_SceneGraphVisualization)
  INCLUDE_DIRECTORIES( ${IGSTK_SOURCE_DIR}/Utilities/SceneGraphVisualization )
ENDIF(IGSTK_USE_SceneGraphVisualization)

#-----------------------------------------------------------------------------
# IGSTK build video imager classes
OPTION(IGSTK_USE_VideoImager "Build video imager classes are under review and not fully tested yet" OFF)

IF(IGSTK_USE_VideoImager)
  MESSAGE("Warning: Video imager classes are under review and not fully tested yet")
ENDIF(IGSTK_USE_VideoImager)

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
  ${ITK_DIR}/Utilities/zlib
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
# Help other projects use IGSTK.
SET(IGSTK_LIBRARY_PATH ${LIBRARY_OUTPUT_PATH})


# Copy the UseIGSTK.cmake file to the binary tree for backward compatability.
CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/UseIGSTK.cmake.in
               ${IGSTK_BINARY_DIR}/UseIGSTK.cmake COPYONLY IMMEDIATE)

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${IGSTK_BINARY_DIR}/IGSTKBuildSettings.cmake)

#-----------------------------------------------------------------------------
# Installation vars.
# IGSTK_INSTALL_BIN_DIR          - binary dir (executables)
# IGSTK_INSTALL_LIB_DIR          - library dir (libs)
# IGSTK_INSTALL_DATA_DIR         - share dir (say, examples, data, etc)
# IGSTK_INSTALL_INCLUDE_DIR      - include dir (headers)
# IGSTK_INSTALL_PACKAGE_DIR      - package/export configuration files

IF(NOT IGSTK_INSTALL_BIN_DIR)
   SET(IGSTK_INSTALL_BIN_DIR
      ${CMAKE_INSTALL_PREFIX}/bin
   )
ENDIF(NOT IGSTK_INSTALL_BIN_DIR)

IF(NOT IGSTK_INSTALL_LIB_DIR)
   SET(IGSTK_INSTALL_LIB_DIR
      ${CMAKE_INSTALL_PREFIX}/lib/IGSTK
   )
ENDIF(NOT IGSTK_INSTALL_LIB_DIR)

IF(NOT IGSTK_INSTALL_DATA_DIR)
   SET(IGSTK_INSTALL_DATA_DIR
      ${CMAKE_INSTALL_PREFIX}/share/IGSTK
   )
ENDIF(NOT IGSTK_INSTALL_DATA_DIR)

IF(NOT IGSTK_INSTALL_INCLUDE_DIR)
   SET(IGSTK_INSTALL_INCLUDE_DIR
      ${CMAKE_INSTALL_PREFIX}/include/IGSTK
   )
ENDIF(NOT IGSTK_INSTALL_INCLUDE_DIR)

IF(NOT IGSTK_INSTALL_PACKAGE_DIR)
   SET(IGSTK_INSTALL_PACKAGE_DIR
      ${IGSTK_INSTALL_LIB_DIR}
#      CACHE INTERNAL ""
   )
ENDIF(NOT IGSTK_INSTALL_PACKAGE_DIR)


# Save library dependencies.
EXPORT_LIBRARY_DEPENDENCIES(${IGSTK_BINARY_DIR}/IGSTKLibraryDepends.cmake)

# Create the IGSTKConfig.cmake file containing the IGSTK configuration.
INCLUDE (${IGSTK_SOURCE_DIR}/igstkGenerateIGSTKConfig.cmake)

# Install some files.
INSTALL(FILES
   ${IGSTK_BINARY_DIR}/igstkConfigure.h
   DESTINATION ${IGSTK_INSTALL_INCLUDE_DIR}
)

INSTALL(FILES
   ${IGSTK_BINARY_DIR}/IGSTKBuildSettings.cmake
   ${IGSTK_BINARY_DIR}/IGSTKLibraryDepends.cmake
   ${IGSTK_BINARY_DIR}/UseIGSTK.cmake
   DESTINATION ${IGSTK_INSTALL_PACKAGE_DIR}
)

#-----------------------------------------------------------------------------
# Set up CPack support, to be able to distribute IGSTK
#
INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "IGSTK ")
SET(CPACK_PACKAGE_VENDOR "ISC")
SET(CPACK_PACKAGE_VERSION_MAJOR "${IGSTK_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${IGSTK_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${IGSTK_VERSION_PATCH}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "IGSTK-${IGSTK_VERSION_MAJOR}.${IGSTK_VERSION_MINOR}")
INCLUDE(CPack)


#-----------------------------------------------------------------------------
# Add compiler flags IGSTK needs to work on this platform.  This must be
# done AFTER the call to CMAKE_EXPORT_BUILD_SETTINGS.
#-----------------------------------------------------------------------------
# ITK requires special compiler flags on some platforms.
if(CMAKE_COMPILER_IS_GNUCXX)
  set(IGSTK_REQUIRED_C_FLAGS "${IGSTK_REQUIRED_C_FLAGS} -Wall -Wno-uninitialized -Wno-unused-parameter")
  set(IGSTK_REQUIRED_CXX_FLAGS "${IGSTK_REQUIRED_CXX_FLAGS} -ftemplate-depth-50 -Wall")
  include(${IGSTK_SOURCE_DIR}/CMake/itkCheckCXXAcceptsFlags.cmake)
  itkCHECK_CXX_ACCEPTS_FLAGS("-Wno-deprecated" CXX_HAS_DEPRECATED_FLAG)
  if(CXX_HAS_DEPRECATED_FLAG)
    set(IGSTK_REQUIRED_CXX_FLAGS "${IGSTK_REQUIRED_CXX_FLAGS} -Wno-deprecated")
  endif(CXX_HAS_DEPRECATED_FLAG)
endif(CMAKE_COMPILER_IS_GNUCXX)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IGSTK_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGSTK_REQUIRED_CXX_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${IGSTK_REQUIRED_LINK_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${IGSTK_REQUIRED_LINK_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${IGSTK_REQUIRED_LINK_FLAGS}")

IF(MSVC_VERSION EQUAL 1400 OR MSVC_VERSION GREATER 1400)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
ENDIF(MSVC_VERSION EQUAL 1400 OR MSVC_VERSION GREATER 1400)

SUBDIRS(Utilities)

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

#-----------------------------------------------------------------------------
# IGSTK uses KWStyle for checking the coding style
OPTION(IGSTK_USE_KWSTYLE "Run KWStyle in order to check for violations of the coding standard." OFF)
MARK_AS_ADVANCED(IGSTK_USE_KWSTYLE)

IF(IGSTK_USE_KWSTYLE)
  FIND_PROGRAM(KWSTYLE_EXECUTABLE
  NAMES KWStyle
  PATHS
  /usr/local/bin
  )
  MARK_AS_ADVANCED(KWSTYLE_EXECUTABLE)

  OPTION(KWSTYLE_USE_VIM_FORMAT "Set KWStyle to generate errors with a VIM-compatible format." OFF)
  OPTION(KWSTYLE_USE_MSVC_FORMAT "Set KWStyle to generate errors with a VisualStudio-compatible format." OFF)

  CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/Utilities/KWStyle/IGSTK.kws.xml.in
               ${IGSTK_BINARY_DIR}/Utilities/KWStyle/IGSTK.kws.xml)

  CONFIGURE_FILE(${IGSTK_SOURCE_DIR}/Utilities/KWStyle/IGSTKFiles.txt.in
               ${IGSTK_BINARY_DIR}/Utilities/KWStyle/IGSTKFiles.txt)

  SET(KWSTYLE_ARGUMENTS -xml ${IGSTK_BINARY_DIR}/Utilities/KWStyle/IGSTK.kws.xml
                        -o ${IGSTK_SOURCE_DIR}/Utilities/KWStyle/IGSTKOverwrite.txt
                        -v
                        -D ${IGSTK_BINARY_DIR}/Utilities/KWStyle/IGSTKFiles.txt
  )

  IF(KWSTYLE_USE_VIM_FORMAT)
    SET(KWSTYLE_ARGUMENTS -vim ${KWSTYLE_ARGUMENTS})
  ENDIF(KWSTYLE_USE_VIM_FORMAT)

  IF(KWSTYLE_USE_MSVC_FORMAT)
    SET(KWSTYLE_ARGUMENTS -msvc ${KWSTYLE_ARGUMENTS})
  ENDIF(KWSTYLE_USE_MSVC_FORMAT)

    ADD_CUSTOM_COMMAND(
    OUTPUT  ${IGSTK_BINARY_DIR}/KWStyleReport.txt
    COMMAND ${KWSTYLE_EXECUTABLE}
    ARGS    ${KWSTYLE_ARGUMENTS}
    COMMENT "Coding Style Checker"
    )
  ADD_CUSTOM_TARGET(StyleCheck DEPENDS ${IGSTK_BINARY_DIR}/KWStyleReport.txt)
ENDIF(IGSTK_USE_KWSTYLE)

# Microsoft Visual Studio 10 needs additional flag in order to compile in
# 64 Bit mode
IF(CMAKE_GENERATOR STREQUAL "Visual Studio 10 Win64")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 10 Win64")

IF( WIN32 )
  SET( IGSTK_SERIAL_PORT_0 "COM1:" CACHE STRING "First serial port device name (Default COM1)")
  SET( IGSTK_SERIAL_PORT_1 "COM2:" CACHE STRING "Second serial port device name (Default COM2)")
  SET( IGSTK_SERIAL_PORT_2 "COM3:" CACHE STRING "Third serial port device name (Default COM3)")
  SET( IGSTK_SERIAL_PORT_3 "COM4:" CACHE STRING "Fourth serial port device name (Default COM4)")
  SET( IGSTK_SERIAL_PORT_4 "COM5:" CACHE STRING "Fifith serial port device name (Default COM5)")
  SET( IGSTK_SERIAL_PORT_5 "COM6:" CACHE STRING "Sixth serial port device name (Default COM6)")
  SET( IGSTK_SERIAL_PORT_6 "COM7:" CACHE STRING "Seventh serial port device name (Default COM7)")
  SET( IGSTK_SERIAL_PORT_7 "COM8:" CACHE STRING "Eighth serial port device name (Default COM7)")
ELSE (WIN32 )
  SET( IGSTK_SERIAL_PORT_0 "/dev/ttyS0" CACHE STRING "First serial port device name  (Default /dev/ttyS0)")
  SET( IGSTK_SERIAL_PORT_1 "/dev/ttyS1" CACHE STRING "Second serial port device name  (Default /dev/ttyS1)")
  SET( IGSTK_SERIAL_PORT_2 "/dev/ttyS2" CACHE STRING "Third serial port device name  (Default /dev/ttyS2)")
  SET( IGSTK_SERIAL_PORT_3 "/dev/ttyS3" CACHE STRING "Fourth serial port device name  (Default /dev/ttyS3)")
  SET( IGSTK_SERIAL_PORT_4 "/dev/ttyS4" CACHE STRING "Fifith serial port device name  (Default /dev/ttyS4)")
  SET( IGSTK_SERIAL_PORT_5 "/dev/ttyS5" CACHE STRING "Sixth serial port device name  (Default /dev/ttyS5)")
  SET( IGSTK_SERIAL_PORT_6 "/dev/ttyS6" CACHE STRING "Seventh serial port device name  (Default /dev/ttyS6)")
  SET( IGSTK_SERIAL_PORT_7 "/dev/ttyS7" CACHE STRING "Eighth serial port device name  (Default /dev/ttyS7)")
ENDIF ( WIN32 )
