# Install script for directory: /Users/jishii/Documents/rep_other/fprime-sw/Fw

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/jishii/Documents/rep_other/fprime-sw/Ref")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "DEBUG")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Buffer/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Com/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Cmd/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Log/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Time/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Tlm/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Prm/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Cfg/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/ComFile/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Comp/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Obj/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Port/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/Types/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/FilePacket/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/cmake-py/F-Prime/Fw/SerializableFile/cmake_install.cmake")

endif()

