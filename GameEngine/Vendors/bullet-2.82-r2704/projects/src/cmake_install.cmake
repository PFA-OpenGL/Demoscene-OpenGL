# Install script for directory: C:/Users/cesar/Documents/GitHub/Demoscene-OpenGL-PFA/GameEngine/Vendors/bullet-2.82-r2704/src

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/BULLET_PHYSICS")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
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

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("C:/Users/cesar/Documents/GitHub/Demoscene-OpenGL-PFA/GameEngine/Vendors/bullet-2.82-r2704/projects/src/BulletSoftBody/cmake_install.cmake")
  INCLUDE("C:/Users/cesar/Documents/GitHub/Demoscene-OpenGL-PFA/GameEngine/Vendors/bullet-2.82-r2704/projects/src/BulletCollision/cmake_install.cmake")
  INCLUDE("C:/Users/cesar/Documents/GitHub/Demoscene-OpenGL-PFA/GameEngine/Vendors/bullet-2.82-r2704/projects/src/BulletDynamics/cmake_install.cmake")
  INCLUDE("C:/Users/cesar/Documents/GitHub/Demoscene-OpenGL-PFA/GameEngine/Vendors/bullet-2.82-r2704/projects/src/LinearMath/cmake_install.cmake")
  INCLUDE("C:/Users/cesar/Documents/GitHub/Demoscene-OpenGL-PFA/GameEngine/Vendors/bullet-2.82-r2704/projects/src/MiniCL/cmake_install.cmake")
  INCLUDE("C:/Users/cesar/Documents/GitHub/Demoscene-OpenGL-PFA/GameEngine/Vendors/bullet-2.82-r2704/projects/src/BulletMultiThreaded/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)
