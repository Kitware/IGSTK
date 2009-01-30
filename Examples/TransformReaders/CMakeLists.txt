PROJECT(TransformReaders)
#
# Source code for the project
#
SET(SRCS
    igstkPrecomputedTransformData.cxx
    igstkTransformXMLFileReaderBase.cxx
    igstkRigidTransformXMLFileReader.cxx
    igstkAffineTransformXMLFileReader.cxx
    igstkPerspectiveTransformXMLFileReader.cxx
    igstkTransformFileReader.cxx
    TransformReaderExample.cxx
)
SET(HDRS
    igstkPrecomputedTransformData.h
    igstkTransformXMLFileReaderBase.h
    igstkRigidTransformXMLFileReader.h
    igstkAffineTransformXMLFileReader.h
    igstkPerspectiveTransformXMLFileReader.h
    igstkTransformFileReader.h
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
   IGSTK
)


ADD_EXECUTABLE(TransformReaderExample ${SRCS} ${HDRS})

TARGET_LINK_LIBRARIES(TransformReaderExample ${LINK_LIBRARIES})
