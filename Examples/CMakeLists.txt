PROJECT( ApplicationModel )

#
#  Define the list of source files
#
SET(APPLICATION_SOURCES
  ApplicationBase.cxx
  Application.cxx
  )


ADD_EXECUTABLE( ApplicationModel main.cxx ${APPLICATION_SOURCES})

FLTK_WRAP_UI(ApplicationModel 
  ApplicationGUI.fl 
  )


INCLUDE_DIRECTORIES(
  ${ApplicationModel_SOURCE_DIR}
  ${ApplicationModel_BINARY_DIR}
  )

TARGET_LINK_LIBRARIES( ApplicationModel IGSTK )

