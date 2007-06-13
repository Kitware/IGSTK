# This project is designed to be built outside the Insight source tree.
PROJECT(HelloWorld)

# Find IGSTK.
FIND_PACKAGE(IGSTK REQUIRED)
IF(IGSTK_FOUND)
  INCLUDE(${IGSTK_USE_FILE})
ENDIF(IGSTK_FOUND)

ADD_EXECUTABLE(HelloWorld HelloWorld.cxx )

TARGET_LINK_LIBRARIES(HelloWorld IGSTK)
