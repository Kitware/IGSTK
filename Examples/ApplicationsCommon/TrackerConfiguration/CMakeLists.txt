PROJECT(TrackerConfiguration)
  
  
SET(TrackerConfiguration_SRCS
  main.cxx
  igstkTrackerConfiguration.cxx
  igstkTrackerController.cxx
  igstkAscensionTrackerConfiguration.cxx
  igstkAuroraTrackerConfiguration.cxx
  igstkMicronTrackerConfiguration.cxx
  igstkPolarisTrackerConfiguration.cxx
)
  
INCLUDE_DIRECTORIES(
  ${TrackerConfiguration_SOURCE_DIR}
  ${TrackerConfiguration_BINARY_DIR}
  )

ADD_EXECUTABLE(TrackerConfiguration ${TrackerConfiguration_SRCS})
TARGET_LINK_LIBRARIES(TrackerConfiguration IGSTKSandbox)


