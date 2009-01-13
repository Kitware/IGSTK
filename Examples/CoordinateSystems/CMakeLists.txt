PROJECT(CoordinateSystems)
  
IF (IGSTKSandbox_USE_FLTK)

  ADD_EXECUTABLE(CoordinateSystems1 CoordinateSystems1.cxx)
  TARGET_LINK_LIBRARIES(CoordinateSystems1 IGSTKSandbox)

  ADD_EXECUTABLE(CoordinateSystems2 CoordinateSystems2.cxx)
  TARGET_LINK_LIBRARIES(CoordinateSystems2 IGSTKSandbox)

  ADD_EXECUTABLE(CoordinateSystems3 CoordinateSystems3.cxx)
  TARGET_LINK_LIBRARIES(CoordinateSystems3 IGSTKSandbox)

ENDIF (IGSTKSandbox_USE_FLTK)

