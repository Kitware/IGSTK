SUBDIRS( 
    ApplicationModel
    LiverTumorSegmentation
    ImageGuidedMagneticTracking     
)

OPTION(USE_QT "Use Qt for application GUI" OFF)
IF(USE_QT)
    SUBDIRS( ApplicationQt )
ENDIF(USE_QT)


