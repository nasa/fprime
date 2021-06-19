####
# Raspberry PI Toolchain
#
# A toolchain file dor the raspberrypi. This toolchain can be used to build against the raspberry pi embedded Linux
# target. In order to use this toolchain, the raspberry pi build tools should be cloned onto a Linux build host. These
# tools are available at: [https://github.com/raspberrypi/tools](https://github.com/raspberrypi/tools)
#
# Typically these tools are cloned to `/opt/rpi/`.  If they are cloned elsewhere, the user must set the environment
# variable `RPI_TOOLCHAIN_DIR` to the full path to the `arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf` directory before
# running CMake or `fprime-util generate`.
#
# e.g. should user install tools ``/home/user1` then the environment variable might be set using
# `export RPI_TOOLCHAIN_DIR=/home/user/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/`
####
# Set system name
set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "arm")

# Location pi toolchain
set(RPI_TOOLCHAIN "$ENV{RPI_TOOLCHAIN_DIR}")
 ("${RPI_TOOLCHAIN}"  "")
    (RPI_TOOLCHAIN "/opt/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf")
()
# Check toolchain directory exists
(NOT EXISTS "${RPI_TOOLCHAIN}")
    (FATAL_ERROR "RPI toolchain not found at ${RPI_TOOLCHAIN}. Install it, set RPI_TOOLCHAIN_DIR environment variable, or choose another toolchain")
()
message(STATUS "Using RPI toolchain at: ${RPI_TOOLCHAIN}")
# specify cross compiler
set(CMAKE_C_COMPILER "${RPI_TOOLCHAIN}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${RPI_TOOLCHAIN}/bin/arm-linux-gnueabihf-g++")

# where target environment
set(CMAKE_FIND_ROOT_PATH  "${RPI_TOOLCHAIN}")

# search programs build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# libraries and headers target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
