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



TARGET_LINK_LIBRARIES( ApplicationModel
    IGSTK 
    )

