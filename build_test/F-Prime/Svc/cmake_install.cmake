# Install script for directory: /Users/jishii/Documents/rep_other/fprime-sw/Svc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/jishii/Documents/rep_other/fprime-sw")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "TESTING")
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
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/Cycle/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/Fatal/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/Ping/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/PolyIf/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/Sched/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/Seq/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/WatchDog/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/ActiveLogger/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/ActiveRateGroup/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/AssertFatalAdapter/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/BufferManager/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/BuffGndSockIf/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/ComLogger/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/CmdDispatcher/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/CmdSequencer/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/FatalHandler/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/FileDownlink/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/FileManager/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/FileUplink/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/GndIf/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/Health/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/LinuxTime/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/LinuxTimer/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/PassiveConsoleTextLogger/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/PassiveTextLogger/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/PolyDb/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/PrmDb/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/RateGroupDriver/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/SocketGndIf/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/Time/cmake_install.cmake")
  include("/Users/jishii/Documents/rep_other/fprime-sw/build_test/F-Prime/Svc/TlmChan/cmake_install.cmake")

endif()

