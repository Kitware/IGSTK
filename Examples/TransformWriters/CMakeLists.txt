PROJECT(TransformWriters)
#
# Source code for the project
#
SET(SRCS
    ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkPrecomputedTransformData.cxx
    igstkTransformXMLFileWriterBase.cxx
    igstkRigidTransformXMLFileWriter.cxx
    igstkTransformFileWriter.cxx
    TransformWriterExample.cxx
)
SET(HDRS
    ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkPrecomputedTransformData.h
    igstkTransformXMLFileWriterBase.h
    igstkRigidTransformXMLFileWriter.h
    igstkTransformFileWriter.h
)

INCLUDE_DIRECTORIES(
${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading
.
)

#
# Link to the following libraries
#
LINK_LIBRARIES (
   IGSTKSandbox
)


ADD_EXECUTABLE(TransformWriterExample ${SRCS} ${HDRS})

TARGET_LINK_LIBRARIES(TransformWriterExample ${LINK_LIBRARIES})
