# Install script for directory: /Users/jishii/Documents/fprime-sw/Ref

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/jishii/Documents/fprime-sw/Ref")
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
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/F-Prime/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/F-Prime/Autocoders/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/F-Prime/Fw/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/F-Prime/Svc/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/F-Prime/Os/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/F-Prime/Drv/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/F-Prime/CFDP/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/F-Prime/Utils/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/Ref/PingReceiver/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/Ref/RecvBuffApp/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/Ref/SendBuffApp/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/Ref/SignalGen/cmake_install.cmake")
  include("/Users/jishii/Documents/fprime-sw/cmake_dicts/Ref/Top/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/jishii/Documents/fprime-sw/cmake_dicts/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
