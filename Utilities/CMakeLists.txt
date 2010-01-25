# Option for using code in the Atamai directory
OPTION(IGSTK_USE_ATAMAI "Use the code from Atamai directory" OFF)
MARK_AS_ADVANCED(IGSTK_USE_ATAMAI)
SET( IGSTK_USE_ATAMAI_CONFIG 0 )
IF( IGSTK_USE_ATAMAI )
    SUBDIRS( Atamai )
    SET( IGSTK_USE_ATAMAI_CONFIG 1 )
ENDIF( IGSTK_USE_ATAMAI )

IF(IGSTKSandbox_USE_MicronTracker )
    SUBDIRS( MicronTracker )
ENDIF( IGSTKSandbox_USE_MicronTracker )

IF(IGSTKSandbox_USE_vtkKWImage )
    SUBDIRS( vtkKWImage )
ENDIF( IGSTKSandbox_USE_vtkKWImage )

IF(IGSTK_USE_SceneGraphVisualization )
    SUBDIRS( SceneGraphVisualization )
ENDIF( IGSTK_USE_SceneGraphVisualization )

IF(IGSTKSandbox_USE_NDICertus )
    SUBDIRS( NDICertus )
ENDIF( IGSTKSandbox_USE_NDICertus )
