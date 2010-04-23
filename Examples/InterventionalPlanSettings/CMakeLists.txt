PROJECT(InterventionalPlanSettings)

#
# Source code for the project
#
SET(SRCS
    igstkTrajectory.cxx
    igstkInterventionalPlan.cxx
    igstkMultiTrajectoryInterventionalPlan.cxx
)

SET(HDRS
    igstkTrajectory.h
    igstkInterventionalPlan.h
    igstkMultiTrajectoryInterventionalPlan.h
)


ADD_LIBRARY( InterventionalPlanSettings ${SRCS} ${HDRS})
