PROJECT(UltrasoundGuidedRFA)
  
SET(UltrasoundGuidedRFA_GUI_SRCS
  UltrasoundGuidedRFA.fl
  )
  
SET(UltrasoundGuidedRFA_SRCS
  main.cxx
  )

INCLUDE_DIRECTORIES(
  ${UltrasoundGuidedRFA_SOURCE_DIR}
  ${UltrasoundGuidedRFA_BINARY_DIR}
  )

FLTK_WRAP_UI(UltrasoundGuidedRFA ${UltrasoundGuidedRFA_GUI_SRCS})
ADD_EXECUTABLE(UltrasoundGuidedRFA ${UltrasoundGuidedRFA_FLTK_UI_SRCS} ${UltrasoundGuidedRFA_SRCS})
TARGET_LINK_LIBRARIES(UltrasoundGuidedRFA IGSTKSandbox)



