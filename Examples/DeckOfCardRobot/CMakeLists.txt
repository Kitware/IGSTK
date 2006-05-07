PROJECT(DeckOfCardRobot)
  
SET(DeckOfCardRobot_GUI_SRCS
  DeckOfCardRobotGUI.fl
  )
  
SET(DeckOfCardRobot_HEADER
  DeckOfCardRobot.h
  DOCR_Registration.h  
  RobotCommunication.h
  igstkImageSpatialObjectVolumeRepresentation.h
)

SET(DeckOfCardRobot_SRCS
  main.cxx
  DeckOfCardRobot.cxx
  DOCR_Registration.cxx
  RobotCommunication.cxx
  igstkImageSpatialObjectVolumeRepresentation.txx
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


