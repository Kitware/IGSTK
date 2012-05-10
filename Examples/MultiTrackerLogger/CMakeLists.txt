PROJECT( MultiTrackerLogger )

SET( MultiTrackerLogger_HEADERS
  MultiTrackerLogger.h
   )

SET( MultiTrackerLogger_SOURCES
  MultiTrackerLogger.cxx
   )

SET( IGDTTrackerLibs
   )

# Set Tracker specific sources and headers (IGDTTrackerHeaders, IGDTTrackerSources)
SET( IGDTTracker_INCLUDES )
SET( IGDTTracker_LIBS )

ADD_DEFINITIONS(-DUSE_POLARISCLASSIC)

IF( IGSTK_USE_AtracsysEasyTrack500 )
	ADD_DEFINITIONS(-DUSE_EASYTRACK)
ENDIF( IGSTK_USE_AtracsysEasyTrack500 )

IF( IGSTK_USE_AXIOS3D_CAMBARB2 )
	ADD_DEFINITIONS(-DUSE_AXIOS3D)
ENDIF( IGSTK_USE_AXIOS3D_CAMBARB2 )

INCLUDE_DIRECTORIES(
	${MultiTrackerLogger_SOURCE_DIR}
	${MultiTrackerLogger_BINARY_DIR}
	${IGDTTracker_INCLUDES}
	)

ADD_EXECUTABLE(MultiTrackerLogger
               ${MultiTrackerLogger_SOURCES}
               ${MultiTrackerLogger_HEADERS}
               ${IGDTTrackerHeaders}
               ${IGDTTrackerSources}
               )

TARGET_LINK_LIBRARIES(MultiTrackerLogger IGSTK ${IGDTTracker_LIBS} )
