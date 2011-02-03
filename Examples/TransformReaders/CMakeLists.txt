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
.
)

ADD_LIBRARY( TransformReaders ${SRCS} ${HDRS})
ADD_EXECUTABLE(TransformReaderExample TransformReaderExample.cxx) 
TARGET_LINK_LIBRARIES(TransformReaderExample TransformReaders IGSTK)
