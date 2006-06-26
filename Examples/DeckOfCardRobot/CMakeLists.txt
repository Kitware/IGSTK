PROJECT(DeckOfCardRobot)
  
SET(DeckOfCardRobot_GUI_SRCS
  DeckOfCardRobotGUI.fl
  )
  
SET(DeckOfCardRobot_HEADER
  DeckOfCardRobot.h
  RobotCommunication.h
  igstkImageSpatialObjectVolumeRepresentation.h
  FiducialSegmentation.h
  FiducialModel.h
  ModelBasedClustering.h
  DeckOfCardRobotRegistrationBase.h
  IterativeClosestPointsBasedRegistration.h
  LandmarkBasedRegistration.h
  ModelBasedRegistration.h
  PCAOnPoints.h
)

SET(DeckOfCardRobot_SRCS
  main.cxx
  DeckOfCardRobot.cxx
  RobotCommunication.cxx
  igstkImageSpatialObjectVolumeRepresentation.txx
  FiducialSegmentation.cxx
  FiducialModel.cxx
  ModelBasedClustering.cxx
  DeckOfCardRobotRegistrationBase.cxx
  IterativeClosestPointsBasedRegistration.cxx
  LandmarkBasedRegistration.cxx
  ModelBasedRegistration.cxx
  PCAOnPoints.cxx
  )

INCLUDE_DIRECTORIES(
  ${DeckOfCardRobot_SOURCE_DIR}
  ${DeckOfCardRobot_BINARY_DIR}
  )

FLTK_WRAP_UI(DeckOfCardRobot ${DeckOfCardRobot_GUI_SRCS})

ADD_EXECUTABLE(DeckOfCardRobot 
  ${DeckOfCardRobot_FLTK_UI_SRCS} 
  ${DeckOfCardRobot_SRCS}
  ${DeckOfCardRobot_HEADER}
)

TARGET_LINK_LIBRARIES(DeckOfCardRobot IGSTKSandbox)


