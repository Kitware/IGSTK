
PROJECT( IGSTK )

SET (LIBRARY_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${IGSTK_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all executables." FORCE)

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})


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
SET(IGSTK_HEADS
  DicomImageReaderBase.h
  DicomImageReader.h
  RegistrationCTBase.h
  igstkClickedPointEvent.h
  igstkImageSliceViewer.h
  igstkInteractorObserver.h
  igstkStateMachine.h
  ImageSliceViewer.h
  vtkFlRenderWindowInteractor.h
  InteractorObserver.h
  ConfidenceConnectedModule.h
  ConnectedThresholdModule.h
  ThresholdLevelSetSegmentationModule.h
  LiverTumorSegmentationBase.h  
#  FastMarchingModule.h
#  GeodesicActiveContourModule.h
#  IsolatedConnectedModule.h
#  WatershedModule.h
  TubeWrappedLine.h
  MotionViewer.h
  FiducialRegistration.h
  Sphere.h
  IGMTImageSliceViewer.h
  IGMTrackingBase.h
  Annotation.h
  Fl_Double_Window_Ex.h
  IGMTTargetViewer.h
  IGMTVolumeViewer.h
  CrossLine.h
  FantasticRegistration.h
  IGMTRegistrationViewer.h
)

SET(IGSTK_SRCS
  DicomImageReaderBase.cxx
  RegistrationCTBase.cxx
  igstkImageSliceViewer.cxx
  ImageSliceViewer.cxx
  vtkFlRenderWindowInteractor.cxx
  InteractorObserver.cxx
  ConfidenceConnectedModule.cxx
  ConnectedThresholdModule.cxx
  ThresholdLevelSetSegmentationModule.cxx
#  FastMarchingModule.cxx
#  GeodesicActiveContourModule.cxx
#  IsolatedConnectedModule.cxx
#  WatershedModule.cxx
  LiverTumorSegmentationBase.cxx  
  TubeWrappedLine.cxx
  MotionViewer.cxx
  FiducialRegistration.cxx
  Sphere.cxx
  IGMTImageSliceViewer.cxx
  IGMTrackingBase.cxx
  Annotation.cxx
  Fl_Double_Window_Ex.cxx
  IGMTTargetViewer.cxx
  IGMTVolumeViewer.cxx
  CrossLine.cxx
  igstkInteractorObserver.cxx
  FantasticRegistration.cxx
  IGMTRegistrationViewer.cxx
  )

SET(AURORA_TRACKER_HEADS
  Tracker.h
  AuroraTracker.h
  CommonNDIDefines.h
  Conversions.h
  CyclicRedundancy.h
  IniFileRW.h
  SerialCommunication.h
)

SET(AURORA_TRACKER_SRCS
  Tracker.cxx
  AuroraTracker.cxx
  Conversions.cxx
  CyclicRedundancy.cxx
  IniFileRW.cxx
  SerialCommunication.cxx
)

ADD_LIBRARY(IGSTK ${IGSTK_HEADS} ${IGSTK_SRCS} ${AURORA_TRACKER_HEADS} ${AURORA_TRACKER_SRCS})

FLTK_WRAP_UI(IGSTK 
  DicomImageReaderGUI.fl
  RegistrationCTGUI.fl
  )

TARGET_LINK_LIBRARIES( IGSTK
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon vtkPatented
    ${FLTK_LIBRARIES}
    )

# Add the testing directories
OPTION(IGSTK_BUILD_TESTING "Test the project" ON)
IF(IGSTK_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  SUBDIRS(Tests)
ENDIF(IGSTK_BUILD_TESTING)


# Add the examples directories
OPTION(IGSTK_BUILD_EXAMPLES "Examples of the project" ON)
IF(IGSTK_BUILD_EXAMPLES)
  SUBDIRS(Examples)
ENDIF(IGSTK_BUILD_EXAMPLES)

#-----------------------------------------------------------------------------
# Configure the default IGSTK_DATA_ROOT for the location of IGSTK Data.
FIND_PATH(IGSTK_DATA_ROOT IGSTKData.readme ${IGSTK_SOURCE_DIR}/Tests/Data $ENV{IGSTK_DATA_ROOT})
MARK_AS_ADVANCED(IGSTK_DATA_ROOT)



