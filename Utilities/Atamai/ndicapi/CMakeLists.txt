PROJECT(VTKNDICAPI)
INCLUDE_REGULAR_EXPRESSION("^(ndicapi|polaris).*$")

INCLUDE_DIRECTORIES(${NDICAPI_SOURCE_DIR})
INCLUDE_DIRECTORIES(${NDICAPI_BINARY_DIR})

SET(common_SRCS
ndicapi.c ndicapi_math.c ndicapi_serial.c ndicapi_thread.c)

ADD_LIBRARY(ndicapi ${common_SRCS})
TARGET_LINK_LIBRARIES( ndicapi )

