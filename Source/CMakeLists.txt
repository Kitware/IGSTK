#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkAuroraTracker.h
  igstkSerialCommunication.h
  igstkSerialCommunicationCommand.h
  igstkMouseTracker.h
)


# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkAuroraTracker.cxx
  igstkSerialCommunication.cxx
  igstkMouseTracker.cxx
)

IF( WIN32 )
  IF(NOT CYGWIN)
    SET(IGSTKSandbox_HEADS ${IGSTKSandbox_HEADS} 
        igstkSerialCommunicationForWindows.h
        )

    SET(IGSTKSandbox_SRCS ${IGSTKSandbox_SRCS} 
        igstkSerialCommunicationForWindows.cxx
        )
  ENDIF(NOT CYGWIN)
ELSE( WIN32 )
    SET(IGSTKSandbox_HEADS ${IGSTKSandbox_HEADS} 
        igstkSerialCommunicationForLinux.h
        )

    SET(IGSTKSandbox_SRCS ${IGSTKSandbox_SRCS} 
        igstkSerialCommunicationForLinux.cxx
        )
ENDIF( WIN32 )

# Adding the IGSTKSandbox library
ADD_LIBRARY(IGSTKSandbox ${IGSTKSandbox_HEADS} ${IGSTKSandbox_SRCS})

# Linking with the IGSTK libraries should bring up all the other dependent
# libraries such as ITK, VTK and FLTK
TARGET_LINK_LIBRARIES( IGSTKSandbox IGSTK)

