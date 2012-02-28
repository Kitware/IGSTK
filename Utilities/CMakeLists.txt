INSTALL(FILES ${IGSTK_BINARY_DIR}/Utilities/IGSTKConfig.cmake
    DESTINATION ${IGSTK_INSTALL_PACKAGE_DIR}
    )

IF( IGSTK_USE_MicronTracker )
    SUBDIRS( MicronTracker )
ENDIF( IGSTK_USE_MicronTracker )

IF( IGSTK_USE_ArucoTracker )
  SUBDIRS( Aruco )
ENDIF( IGSTK_USE_ArucoTracker )

SUBDIRS( SceneGraphVisualization )




