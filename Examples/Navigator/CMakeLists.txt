PROJECT(Navigator)

INCLUDE_DIRECTORIES (
  ${IGSTK_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders
  ${IGSTK_SOURCE_DIR}/Examples/Tools
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
  ${IGSTK_SOURCE_DIR}/Examples/Tools/igstkFiducialsPlan.h
  ${IGSTK_SOURCE_DIR}/Examples/Tools/igstkFiducialsPlan.cxx
  ${IGSTK_SOURCE_DIR}/Examples/Tools/igstkFiducialsPlanIO.h
  ${IGSTK_SOURCE_DIR}/Examples/Tools/igstkFiducialsPlanIO.cxx
  ${IGSTK_SOURCE_DIR}/Examples/Tools/igstkPathPlan.h
  ${IGSTK_SOURCE_DIR}/Examples/Tools/igstkPathPlan.cxx
  ${IGSTK_SOURCE_DIR}/Examples/Tools/igstkPathPlanIO.h
  ${IGSTK_SOURCE_DIR}/Examples/Tools/igstkPathPlanIO.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkPrecomputedTransformData.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkTransformXMLFileReaderBase.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkRigidTransformXMLFileReader.cxx
  ${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkTransformFileReader.cxx

${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkTrackerConfigurationXMLFileReaderBase.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisWirelessConfigurationXMLFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisVicraConfigurationXMLFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisSpectraConfigurationXMLFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisHybridConfigurationXMLFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkAuroraConfigurationXMLFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkAscensionConfigurationXMLFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkMicronConfigurationXMLFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkTrackerConfigurationFileReader.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkTrackerConfiguration.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkPolarisTrackerConfiguration.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkAuroraTrackerConfiguration.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkAscensionTrackerConfiguration.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkMicronTrackerConfiguration.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkTrackerController.cxx
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkTrackerConfigurationXMLFileReaderBase.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisWirelessConfigurationXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisVicraConfigurationXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisSpectraConfigurationXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkPolarisHybridConfigurationXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkAscensionConfigurationXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkAuroraConfigurationXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkMicronConfigurationXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/configurationFileReaders/igstkTrackerConfigurationFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkTrackerConfiguration.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkPolarisTrackerConfiguration.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkAuroraTrackerConfiguration.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkAscensionTrackerConfiguration.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkMicronTrackerConfiguration.h
${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration/igstkTrackerController.h
${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkPrecomputedTransformData.h
${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkTransformXMLFileReaderBase.h
${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkRigidTransformXMLFileReader.h
${IGSTK_SOURCE_DIR}/Examples/TransformReaders/igstkTransformFileReader.h

)
  
IF(Navigator_USE_MicronTracker)
  SET(Navigator_SRCS ${Navigator_SRCS}
  # MicronTracker related classes
  )
ENDIF(Navigator_USE_MicronTracker)

INCLUDE_DIRECTORIES(
  ${Navigator_SOURCE_DIR}
  ${Navigator_BINARY_DIR}
  ${IGSTK_SOURCE_DIR}/Examples/TrackerConfiguration
  )

FLTK_WRAP_UI(Navigator ${Navigator_GUI_SRCS})
ADD_EXECUTABLE(Navigator ${Navigator_FLTK_UI_SRCS} ${Navigator_SRCS})
TARGET_LINK_LIBRARIES(Navigator IGSTK )

