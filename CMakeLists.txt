
PROJECT( IGSTK )

SET (LIBRARY_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all executables." FORCE)

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
FIND_PACKAGE(FLTK)
IF (FLTK_FOUND)
  INCLUDE_DIRECTORIES (${FLTK_INCLUDE_DIR})
ELSE (FLTK_FOUND)
  MESSAGE( FATAL_ERROR "This application requires FLTK. One of these components is missing. Please verify configuration")
ENDIF (FLTK_FOUND)

# Add the testing directories
IF(IGSTK_BUILD_TESTING)
  #  SUBDIRS(Tests)
ENDIF(IGSTK_BUILD_TESTING)

# Add the examples directories
OPTION(IGSTK_BUILD_EXAMPLES "Examples of the project" ON)
IF(IGSTK_BUILD_EXAMPLES)
  #  SUBDIRS(Examples)
ENDIF(IGSTK_BUILD_EXAMPLES)

#-----------------------------------------------------------------------------
# Configure the default IGSTK_DATA_ROOT for the location of IGSTK Data.
#FIND_PATH(IGSTK_DATA_ROOT IGSTKData.readme ${IGSTK_SOURCE_DIR}/Tests/Data $ENV{IGSTK_DATA_ROOT})
#MARK_AS_ADVANCED(IGSTK_DATA_ROOT)



ENDIF(NOT IGSTK_SETUP_DASHBOARD)
