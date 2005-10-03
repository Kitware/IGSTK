
SET(IGSTKSandbox_TESTS ${CXX_TEST_PATH}/igstkSandboxTests)

SET(IGSTKSandbox_TEST_OUTPUT_DIR "${IGSTKSandbox_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${IGSTKSandbox_TEST_OUTPUT_DIR})

SET(IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR "${IGSTKSandbox_TEST_OUTPUT_DIR}/StateMachineDiagrams")
MAKE_DIRECTORY(${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})

SET(IGSTK_DICOM_TEST_OUTPUT_DIR "${IGSTKSandbox_TEST_OUTPUT_DIR}/DICOM")
MAKE_DIRECTORY(${IGSTK_DICOM_TEST_OUTPUT_DIR})

#-----------------------------------------------------------------------------
# Configure the default IGSTKSandbox_DATA_ROOT for the location of IGSTKSandbox Data.
FIND_PATH(IGSTKSandbox_DATA_ROOT IGSTKSandboxData.readme ${IGSTKSandbox_SOURCE_DIR}/Testing/Data $ENV{IGSTKSandbox_DATA_ROOT})

#-----------------------------------------------------------------------------
# Configure the dot tool from graphviz in order to generate the State Machine Diagrams 
FIND_PROGRAM(DOT
  dot
  "C:/Program Files/ATT/Graphviz/bin"
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\ATT\\Graphviz;InstallPath]/bin
)
IF(NOT DOT_PATH)
  GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
ENDIF(NOT DOT_PATH)

FILE(GLOB STATE_MACHINE_DOT_FILES "${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR}/*.dot")

  FOREACH(dotfile ${STATE_MACHINE_DOT_FILES})
    GET_FILENAME_COMPONENT(Imagebase ${dotfile} NAME)
    GET_FILENAME_COMPONENT(ImageBase ${Imagebase} NAME_WE)
    GET_FILENAME_COMPONENT(ImagePath ${dotfile} PATH)
    SET(pngfile ${ImagePath}/${ImageBase}.png)
    ADD_CUSTOM_COMMAND( 
    SOURCE    ${dotfile}
    COMMAND   ${DOT}
    ARGS      -Tpng 
              -o ${pngfile}
              ${dotfile}
    TARGET    StateMachineDiagrams
    OUTPUTS   ${pngfile}
    )
    SET(DOT_PNG_DEPS ${DOT_PNG_DEPS} ${pngfile})
  ENDFOREACH(dotfile)

  ADD_CUSTOM_TARGET(StateMachineDiagrams ALL DEPENDS ${DOT_PNG_DEPS})


#-----------------------------------------------------------------------------
MARK_AS_ADVANCED(IGSTKSandbox_DATA_ROOT)

INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_SOURCE_DIR}
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/Source
  ${IGSTKSandbox_SOURCE_DIR}/Testing
  ${IGSTKSandbox_BINARY_DIR}/Testing
  )

ADD_TEST(igstkMouseTrackerTest ${IGSTKSandbox_TESTS} igstkMouseTrackerTest)
ADD_TEST(igstkPolarisTrackerSimulatedTest ${IGSTKSandbox_TESTS} igstkPolarisTrackerSimulatedTest)
ADD_TEST(igstkLandmarkRegistrationTest ${IGSTKSandbox_TESTS} igstkLandmarkRegistrationTest)

IF(IGSTK_TEST_POLARIS_ATTACHED)
ADD_TEST(igstkPolarisTrackerTest ${IGSTKSandbox_TESTS} igstkPolarisTrackerTest)
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)


IF (IGSTKSandbox_DATA_ROOT) 
   ADD_TEST( igstkDICOMImageReaderTest ${IGSTKSandbox_TESTS} igstkDICOMImageReaderTest 
       ${IGSTKSandbox_DATA_ROOT}/E000192 )

   ADD_TEST( igstkCTImageReaderTest ${IGSTKSandbox_TESTS} igstkCTImageReaderTest 
       ${IGSTKSandbox_DATA_ROOT}/E000192 )

   ADD_TEST( igstkMRImageReaderTest ${IGSTKSandbox_TESTS} igstkMRImageReaderTest 
       ${IGSTKSandbox_DATA_ROOT}/E000192 )

   ADD_TEST( igstkCTImageSpatialObjectReadingAndRepresentationTest ${IGSTKSandbox_TESTS} igstkCTImageSpatialObjectReadingAndRepresentationTest 
       ${IGSTKSandbox_DATA_ROOT}/E000192 )

ENDIF(IGSTKSandbox_DATA_ROOT)

ADD_TEST( igstkImageReaderTest ${IGSTKSandbox_TESTS} igstkImageReaderTest )
ADD_TEST( igstkDICOMImageReaderErrorsTest ${IGSTKSandbox_TESTS} igstkDICOMImageReaderErrorsTest 
          ${IGSTK_DICOM_TEST_OUTPUT_DIR})
ADD_TEST( igstkImageSpatialObjectTest ${IGSTKSandbox_TESTS} igstkImageSpatialObjectTest )
ADD_TEST( igstkCTImageSpatialObjectTest ${IGSTKSandbox_TESTS} igstkCTImageSpatialObjectTest )
ADD_TEST( igstkMRImageSpatialObjectTest ${IGSTKSandbox_TESTS} igstkMRImageSpatialObjectTest )
ADD_TEST( igstkImageSpatialObjectRepresentationTest ${IGSTKSandbox_TESTS} igstkImageSpatialObjectRepresentationTest )
ADD_TEST( igstkCTImageSpatialObjectRepresentationTest ${IGSTKSandbox_TESTS} igstkCTImageSpatialObjectRepresentationTest )
ADD_TEST( igstkStringEventTest ${IGSTKSandbox_TESTS} igstkStringEventTest )

SET(BasicTests_SRCS
  igstkMouseTrackerTest.cxx
  igstkPolarisTrackerSimulatedTest.cxx
  igstkLandmarkRegistrationTest.cxx
  igstkImageReaderTest.cxx
  igstkCTImageReaderTest.cxx
  igstkMRImageReaderTest.cxx
  igstkDICOMImageReaderTest.cxx
  igstkDICOMImageReaderErrorsTest.cxx
  igstkCTImageSpatialObjectTest.cxx
  igstkCTImageSpatialObjectReadingAndRepresentationTest.cxx 
  igstkMRImageSpatialObjectTest.cxx
  igstkImageSpatialObjectTest.cxx
  igstkImageSpatialObjectRepresentationTest.cxx
  igstkCTImageSpatialObjectRepresentationTest.cxx
  igstkMRImageSpatialObjectRepresentationTest.cxx
  igstkStringEventTest.cxx
  )

IF(IGSTK_TEST_POLARIS_ATTACHED)
  SET(BasicTests_SRCS ${BasicTests_SRCS}
    igstkPolarisTrackerTest.cxx
  )
ENDIF(IGSTK_TEST_POLARIS_ATTACHED)

IF (IGSTK_USE_ATAMAI)
  ADD_TEST(igstkAtamaiNDITrackerTest ${IGSTKSandbox_TESTS} igstkAtamaiNDITrackerTest)
  SET(BasicTests_SRCS ${BasicTests_SRCS} igstkAtamaiNDITrackerTest)
ENDIF (IGSTK_USE_ATAMAI)


ADD_EXECUTABLE(igstkSandboxTests igstkSandboxTests.cxx ${BasicTests_SRCS})

TARGET_LINK_LIBRARIES(igstkSandboxTests IGSTKSandbox)

ADD_EXECUTABLE(igstkSystemInformation igstkSystemInformation.cxx)
ADD_TEST(igstkSystemInformation ${EXECUTABLE_OUTPUT_PATH}/igstkSystemInformation)
TARGET_LINK_LIBRARIES(igstkSystemInformation vtkCommon)


ADD_EXECUTABLE(igstkStateMachineExportTest igstkStateMachineExportTest.cxx)
ADD_TEST(igstkStateMachineExportTest ${EXECUTABLE_OUTPUT_PATH}/igstkStateMachineExportTest ${IGSTK_STATE_MACHINE_DIAGRAMS_OUTPUT_DIR})
TARGET_LINK_LIBRARIES(igstkStateMachineExportTest IGSTKSandbox)

# Configure a header needed by SystemInformation.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/igstkSystemInformation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/igstkSystemInformation.h"
               @ONLY IMMEDIATE)

