PROJECT(TrackerConfiguration)
  
  
SET(TrackerConfiguration_SRCS
  main.cxx
  igstkTrackerConfigurationParser.cxx
  igstkTrackerConfigurationComposer.cxx
 # igstkAuroraTrackerConfigurationComposer.cxx
  igstkTrackerConfiguration.cxx
  igstkTrackerController.cxx
  igstkMedSafeTrackerConfiguration.cxx
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


