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

IF( IGSTKSandbox_USE_POLARIS_CLASSIC )
	ADD_DEFINITIONS(-DIGDT_USE_OLDPOLARIS)
ENDIF( IGSTKSandbox_USE_POLARIS_CLASSIC )

IF( IGSTKSandbox_USE_AtracsysEasyTrack500 )
	ADD_DEFINITIONS(-DIGDT_USE_EASYTRACK)
ENDIF( IGSTKSandbox_USE_AtracsysEasyTrack500 )

IF( IGSTKSandbox_USE_AXIOS3D_CAMBARB2 )
	ADD_DEFINITIONS(-DIGDT_USE_AXIOS3D)
ENDIF( IGSTKSandbox_USE_AXIOS3D_CAMBARB2 )

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

TARGET_LINK_LIBRARIES(MultiTrackerLogger IGSTKSandbox ${IGDTTracker_LIBS} )
