
PROJECT( IGSTK )

SET (LIBRARY_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all executables." FORCE)



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


INCLUDE_DIRECTORIES(
  ${IGSTK_SOURCE_DIR}
  ${IGSTK_BINARY_DIR}
  )

#
#  Define the list of source files
#
SET(IGSTK_SRCS
  DicomImageReaderBase.cxx
  RegistrationCTBase.cxx
  ImageSliceViewer.cxx
  vtkFlRenderWindowInteractor.cxx
  InteractorObserver.cxx
  )


ADD_LIBRARY(IGSTK ${IGSTK_SRCS})

FLTK_WRAP_UI(IGSTK 
  DicomImageReaderGUI.fl
  RegistrationCTGUI.fl
  )

TARGET_LINK_LIBRARIES( IGSTK
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon 
    ${FLTK_LIBRARIES}
    )

# Add the testing directories
OPTION(IGSTK_BUILD_TESTING "Test the project" OFF)
IF(IGSTK_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  SUBDIRS(Tests)
ENDIF(IGSTK_BUILD_TESTING)


