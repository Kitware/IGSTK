PROJECT(TransformWriters)
#
# Source code for the project
#
SET(SRCS
    igstkTransformXMLFileWriterBase.cxx
    igstkRigidTransformXMLFileWriter.cxx
    igstkAffineTransformXMLFileWriter.cxx
    igstkTransformFileWriter.cxx
)
SET(HDRS
    igstkTransformXMLFileWriterBase.h
    igstkRigidTransformXMLFileWriter.h
    igstkAffineTransformXMLFileWriter.h
    igstkTransformFileWriter.h
)

INCLUDE_DIRECTORIES(
${IGSTK_SOURCE_DIR}/Examples/TransformReaders
.
)

ADD_LIBRARY( TransformWriters ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(TransformWriters  TransformReaders IGSTK )

ADD_EXECUTABLE(TransformWriterExample TransformWriterExample.cxx) 
TARGET_LINK_LIBRARIES(TransformWriterExample TransformWriters )
