PROJECT(InterventionalPlanSettingsIO)
#
# Source code for the project
#
SET(SRCS
    igstkInterventionalPlanXMLFileReaderBase.cxx
    igstkMultiTrajectoryPlanXMLFileReader.cxx
    igstkInterventionalPlanXMLFileWriterBase.cxx
    igstkMultiTrajectoryPlanXMLFileWriter.cxx
)
SET(HDRS
    igstkInterventionalPlanXMLFileReaderBase.h
    igstkMultiTrajectoryPlanXMLFileReader.h
    igstkInterventionalPlanXMLFileWriterBase.h
    igstkMultiTrajectoryPlanXMLFileWriter.h
)

#
#include directories for project
#
INCLUDE_DIRECTORIES(
  ${IGSTKSandbox_SOURCE_DIR}/Examples/InterventionalPlanSettings
  ${IGSTKSandbox_SOURCE_DIR}/Examples/InterventionalPlanSettingsIO
)


ADD_LIBRARY( InterventionalPlanSettingsIO ${SRCS} ${HDRS})

#
# Link to the following libraries
#
LINK_LIBRARIES (
  IGSTKSandbox
  InterventionalPlanSettings
  InterventionalPlanSettingsIO
)

ADD_EXECUTABLE(MultiTrajectoryPlanReaderExample MultiTrajectoryPlanReaderExample.cxx)
TARGET_LINK_LIBRARIES(MultiTrajectoryPlanReaderExample ${LINK_LIBRARIES})

ADD_EXECUTABLE(MultiTrajectoryPlanWriterExample MultiTrajectoryPlanWriterExample.cxx)
TARGET_LINK_LIBRARIES(MultiTrajectoryPlanWriterExample ${LINK_LIBRARIES})

ADD_EXECUTABLE(MultiTrajectoryPlanReaderWriterExample MultiTrajectoryPlanReaderWriterExample.cxx)
TARGET_LINK_LIBRARIES(MultiTrajectoryPlanReaderWriterExample ${LINK_LIBRARIES})
