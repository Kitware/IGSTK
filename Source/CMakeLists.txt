#
#  Define the list of source files forming the IGSTKSandbox Library
#

# The header files
SET(IGSTKSandbox_HEADS
  igstkAuroraTracker.h
  igstkNDICyclicRedundancy.h
  igstkSerialCommunication.h
  igstkSerialCommunicationCommand.h
  igstkMouseTracker.h
  itkLogger.h
  itkStdStreamLogOutput.h
  itkThreadLogger.h
  itkRealtimeClock.h
  itkMultipleLogOutput.h
  itkLogOutput.h
  itkLogMacro.h
  itkLoggerOutput.h
  itkLoggerManager.h
  igstkVTKLoggerOutput.h
  igstkFLTKTextLogOutput.h
  igstkFLTKTextBufferLogOutput.h
)


# The implementation files
SET(IGSTKSandbox_SRCS  
  igstkAuroraTracker.cxx
  igstkNDICyclicRedundancy.cxx
  igstkSerialCommunication.cxx
  igstkMouseTracker.cxx
  itkLogger.cxx
  itkStdStreamLogOutput.cxx
  itkThreadLogger.cxx
  itkRealtimeClock.cxx
  itkMultipleLogOutput.cxx
  itkLoggerOutput.cxx
  itkLoggerManager.cxx
  igstkVTKLoggerOutput.cxx
  igstkFLTKTextLogOutput.cxx
  igstkFLTKTextBufferLogOutput.cxx
)

# Variable for holding extra libraries
SET(EXTRA_LIBS)

# executed if Atamai classes in Utilities are used
IF( IGSTK_USE_ATAMAI )
  SET(IGSTKSandbox_HEADS ${IGSTKSandbox_HEADS}
    igstkAtamaiNDITracker.h
  )
  SET(IGSTKSandbox_SRCS ${IGSTKSandbox_SRCS}
    igstkAtamaiNDITracker.cxx
  )
  INCLUDE_DIRECTORIES(
    ${ATAMAITRACKING_SOURCE_DIR}
    ${NDICAPI_SOURCE_DIR}
  )
  SET(EXTRA_LIBS ${EXTRA_LIBS} vtkAtamaiTracking)
ENDIF( IGSTK_USE_ATAMAI )


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
TARGET_LINK_LIBRARIES( IGSTKSandbox IGSTK ${EXTRA_LIBS} )
