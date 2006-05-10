INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_SOURCE_DIR}
  ${IGSTKSandbox_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Source
  ${IGSTKSandbox_BINARY_DIR}/Source
  )

SUBDIRS( 
  TwoViews
  FourViewsTrackingWithCT
  FourViews
  UltrasoundGuidedRFA
  NeedleBiopsy
  LandmarkRegistration
  DICOMImageReader
  DeckOfCardRobot
  View
  )
