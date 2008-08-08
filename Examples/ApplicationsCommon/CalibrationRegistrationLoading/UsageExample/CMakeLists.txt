cmake_minimum_required(VERSION 2.4)

PROJECT(CalibrationRegistrationLoadingExample)

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
#  FIND IGSTK
#
FIND_PACKAGE(IGSTK)
IF (IGSTK_USE_FILE)
  INCLUDE (${IGSTK_USE_FILE})
ELSE (IGSTK_USE_FILE)
  MESSAGE( FATAL_ERROR "This application requires IGSTK. One of these components is missing. Please verify configuration")
ENDIF (IGSTK_USE_FILE)



#
# Source code for the project
#
SET(SRCS
    ../igstkPrecomputedTransformData.cxx
    ../igstkTransformXMLFileReaderBase.cxx
    ../igstkRigidTransformXMLFileReader.cxx
    ../igstkAffineTransformXMLFileReader.cxx
    ../igstkTransformFileReader.cxx
    main.cxx
)
SET(HDRS
  ../igstkPrecomputedTransformData.h
  ../igstkTransformXMLFileReaderBase.h
  ../igstkRigidTransformXMLFileReader.h
  ../igstkAffineTransformXMLFileReader.h
  ../igstkTransformFileReader.h
)

#
#All of the header files are one directory up
#
INCLUDE_DIRECTORIES(
..
)

#
# Link to the following libraries
#
LINK_LIBRARIES (
  ${ITK_LIBRARIES}
  IGSTK.lib
)


ADD_EXECUTABLE(CalibrationRegistrationLoadingExample ${SRCS} ${HDRS})

TARGET_LINK_LIBRARIES(CalibrationRegistrationLoadingExample ${LINK_LIBRARIES})
