
PROJECT (IGMTracking)

#
#  Define the list of source files
#

SET (IGMTracking_HEADS
	IGMTracking.h	
  SkeletonModule.h
  DICOMFile.h
 	)

SET (IGMTracking_SRCS
	IGMTracking.cxx
	DICOMFile.cxx	  
 	)

ADD_EXECUTABLE (IGMTracking main.cxx 
	${IGMTracking_HEADS} 
	${IGMTracking_SRCS}
	)

FLTK_WRAP_UI (IGMTracking
	IGMTrackingGUI.fl 
	BrowserGUI.fl 
	MessageWindowGUI.fl 
	PortParametersGUI.fl
	LayoutGUI.fl
	ProbeParametersGUI.fl
  ImageFusionGUI.fl
	SegParametersGUI.fl
  RegParametersGUI.fl
	)


TARGET_LINK_LIBRARIES (IGMTracking IGSTK)

