
SET(IGSTK_TESTS ${CXX_TEST_PATH}/igstkTests)

SET(IGSTK_TEST_OUTPUT_DIR "${IGSTK_BINARY_DIR}/Tests/Temporary")
MAKE_DIRECTORY(${IGSTK_TEST_OUTPUT_DIR})

#-----------------------------------------------------------------------------
# Configure the default IGSTK_DATA_ROOT for the location of IGSTK Data.
FIND_PATH(IGSTK_DATA_ROOT IGSTKData.readme ${IGSTK_SOURCE_DIR}/Tests/Data $ENV{IGSTK_DATA_ROOT})
MARK_AS_ADVANCED(IGSTK_DATA_ROOT)


#IF(ITK_DATA_ROOT)

ADD_TEST(DicomImageReaderTest ${IGSTK_TESTS} --compare
	${IGSTK_DATA_ROOT}/Baseline/DicomImageReader_pngInput.png 
	${IGSTK_TEST_OUTPUT_DIR}/DicomImageReader_pngInput.png
	DicomImageReaderTest 
	${IGSTK_DATA_ROOT}/Input/DicomImageReader_InputSerie.dcm 
	${IGSTK_TEST_OUTPUT_DIR}/DicomImageReader_pngInput.png
	)

ADD_TEST(LiverTumorSegmentationBaseTest ${IGSTK_TESTS} --compare
	${IGSTK_DATA_ROOT}/Baseline/LiverTumorSegmentationBase_Result.gipl 
	${IGSTK_TEST_OUTPUT_DIR}/LiverTumorSegmentationBase_NewResult.gipl
	LiverTumorSegmentationBaseTest 
	${IGSTK_DATA_ROOT}/Input/LiverTumorSegmentationBase_Image.gipl 
	${IGSTK_TEST_OUTPUT_DIR}/LiverTumorSegmentationBase_NewResult.gipl
	)

ADD_TEST(DicomImageReaderTestDirectory ${IGSTK_TESTS} 
	DicomImageReaderTestDirectory 
	${IGSTK_DATA_ROOT}/Baseline/DicomDirectory
	2
	1.3.12.2.1107.5.1.4.24515.4.0.6770456214062487
	1.3.12.2.1107.5.1.4.24515.4.0.6771144821591421
	)


ADD_TEST(igstkStateMachineTest ${IGSTK_TESTS} igstkStateMachineTest)

ADD_TEST(RegistrationCTTest ${IGSTK_TESTS} RegistrationCTTest)

ADD_TEST(ImageSliceViewerTest ${IGSTK_TESTS} ImageSliceViewerTest)

ADD_TEST(MotionViewerTest ${IGSTK_TESTS} MotionViewerTest)

ADD_TEST(ICPRegistrationTest ${IGSTK_TESTS} 
	ICPRegistrationTest 
	${IGSTK_DATA_ROOT}/Input/FixedPoints.dat
	${IGSTK_DATA_ROOT}/Input/MovingPoints.dat
	)
ADD_TEST(igstkEllipseEstimatorTest ${IGSTK_TESTS} igstkEllipseEstimatorTest)

#ENDIF(ITK_DATA_ROOT)

SET(BasicTests_SRCS
  DicomImageReaderTest.cxx
  DicomImageReaderTestDirectory.cxx
  RegistrationCTTest.cxx
  ImageSliceViewerTest.cxx
  LiverTumorSegmentationBaseTest.cxx
  MotionViewerTest.cxx
  ICPRegistrationTest.cxx
  igstkStateMachineTest
  igstkEllipseEstimatorTest.cxx  
  )

ADD_EXECUTABLE(igstkTests igstkTests.cxx ${BasicTests_SRCS})
TARGET_LINK_LIBRARIES(igstkTests IGSTK)

ADD_EXECUTABLE(SystemInformation SystemInformation.cxx)
ADD_TEST(SystemInformation ${EXECUTABLE_OUTPUT_PATH}/SystemInformation)
TARGET_LINK_LIBRARIES(SystemInformation vtkCommon)


# Configure a header needed by SystemInformation.
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SystemInformation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SystemInformation.h"
               @ONLY IMMEDIATE)

