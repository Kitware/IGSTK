
PROJECT (IGMTracking)

#
#  Define the list of source files
#

SET (IGMTracking_HEADS
	IGMTracking.h
	)

SET (IGMTracking_SRCS
	IGMTracking.cxx
	)

ADD_EXECUTABLE (IGMTracking main.cxx 
	${IGMTracking_HEADS} 
	${IGMTracking_SRCS}
	)

FLTK_WRAP_UI (IGMTracking
	IGMTrackingGUI.fl 
	DicomImageReaderGUI.fl 
	BrowserGUI.fl 
	MessageWindowGUI.fl 
	PortParametersGUI.fl
	)

TARGET_LINK_LIBRARIES (IGMTracking IGSTK)

