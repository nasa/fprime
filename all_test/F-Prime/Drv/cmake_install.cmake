# Install script for directory: /Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/Drv

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/DataTypes/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/GpioDriverPorts/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/SerialDriverPorts/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/SpiDriverPorts/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/I2cDriverPorts/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/BlockDriver/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/ByteStreamDriverModel/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/LinuxGpioDriver/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/LinuxSerialDriver/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/LinuxSpiDriver/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/LinuxI2cDriver/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/Ip/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/TcpClient/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/TcpServer/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/Udp/cmake_install.cmake")
  include("/Users/rohand/jpl/fprime/test-env/fprime-fork/fprime/all_test/F-Prime/Drv/SocketIpDriver/cmake_install.cmake")

endif()

