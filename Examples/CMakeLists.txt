SUBDIRS( 
  ApplicationModel
  LiverTumorSegmentation
  )


IF(WIN32)
  SUBDIRS( 
    ImageGuidedMagneticTracking
    )
ENDIF(WIN32)


