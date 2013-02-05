cmake_minimum_required(VERSION 2.6)

project(ARUCO)

#
# Aruco version number.
#
set(${PROJECT_NAME}_VERSION_MAJOR "1")
set(${PROJECT_NAME}_VERSION_MINOR "1")
set(${PROJECT_NAME}_VERSION_PATCH "0")

set(ARUCO_HDRS
    aruco.h
    cameraparameters.h
    arucofidmarkers.h
    cvdrawingutils.h
    exports.h
    marker.h
    markerdetector.h
)

set(ARUCO_SRCS
    cameraparameters.cpp
    arucofidmarkers.cpp
    cvdrawingutils.cpp
    marker.cpp
    markerdetector.cpp
)

add_library(aruco ${ARUCO_HDRS} ${ARUCO_SRCS})
add_executable(aruco_create_marker aruco_create_marker.cpp)

set_target_properties(aruco PROPERTIES
                      VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}
                      SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR}
)

target_link_libraries(aruco ${OpenCV_LIBS})
target_link_libraries(aruco_create_marker aruco ${OpenCV_LIBS})
