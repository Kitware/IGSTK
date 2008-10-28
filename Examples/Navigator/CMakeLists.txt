PROJECT(Navigator)

INCLUDE_DIRECTORIES (
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading

) 
 
SET(Navigator_GUI_SRCS
    NavigatorGUI.fl
   )
  
SET(Navigator_SRCS
  main.cxx
  Navigator.h
  Navigator.cxx
  NavigatorQuadrantViews.h
  NavigatorQuadrantViews.cxx
  igstkFiducialsPlan.h
  igstkFiducialsPlan.cxx
  igstkFiducialsPlanIO.h
  igstkFiducialsPlanIO.cxx
  igstkPathPlan.h
  igstkPathPlan.cxx
  igstkPathPlanIO.h
  igstkPathPlanIO.cxx

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkPrecomputedTransformData.cxx

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkTransformXMLFileReaderBase.cxx

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkRigidTransformXMLFileReader.cxx

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkTransformFileReader.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkTrackerConfiguration.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkPolarisTrackerConfiguration.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkAuroraTrackerConfiguration.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkMicronTrackerConfiguration.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkMedSafeTrackerConfiguration.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkAscensionTrackerConfiguration.cxx
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkTrackerController.cxx

  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkTrackerConfiguration.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkPolarisTrackerConfiguration.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkAuroraTrackerConfiguration.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkMicronTrackerConfiguration.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkMedSafeTrackerConfiguration.h
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration/igstkTrackerController.h

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkPrecomputedTransformData.h

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkTransformXMLFileReaderBase.h

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkRigidTransformXMLFileReader.h

${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/CalibrationRegistrationLoading/igstkTransformFileReader.h

  )
  
IF(Navigator_USE_MicronTracker)
  SET(Navigator_SRCS ${Navigator_SRCS}
  # MicronTracker related classes
  )
ENDIF(Navigator_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${Navigator_SOURCE_DIR}
  ${Navigator_BINARY_DIR}
  ${IGSTKSandbox_SOURCE_DIR}/Examples/ApplicationsCommon/TrackerConfiguration
  )

FLTK_WRAP_UI(Navigator ${Navigator_GUI_SRCS})
ADD_EXECUTABLE(Navigator ${Navigator_FLTK_UI_SRCS} ${Navigator_SRCS})
TARGET_LINK_LIBRARIES(Navigator IGSTKSandbox )

