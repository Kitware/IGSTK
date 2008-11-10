# Install script for directory: /home/ozgur/workspace/IGSTKSandbox/Examples

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/LandmarkRegistration/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/LandmarkRegistrationErrorEstimation/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/DICOMImageReader/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/PivotCalibration/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/PrincipalAxisCalibration/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/PivotCalibrationReader/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/SpatialObjects/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/ApplicationsCommon/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/OneViewAndTrackingNewUsingFLTKWidget/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/TwoViews/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/FourViewsTrackingWithCT/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/FourViews/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/DeckOfCardRobot/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/View/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/Logging/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/Navigator/cmake_install.cmake")
  INCLUDE("/home/ozgur/workspace/IGSTKSandbox/Examples/WorkingVolumeTester/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

