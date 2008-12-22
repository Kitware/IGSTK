PROJECT(MicronTracker)

IF( IGSTKSandbox_USE_MicronTracker )  
  ADD_EXECUTABLE(MicronTrackerExample MicronTracker.cxx)
  TARGET_LINK_LIBRARIES(MicronTrackerExample IGSTKSandbox)
ENDIF( IGSTKSandbox_USE_MicronTracker )

