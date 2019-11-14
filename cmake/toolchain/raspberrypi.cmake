# Set system name
set(CMAKE_SYSTEM_NAME "arm-linux-gnueabihf")

# Location of pi toolchain
set(PI_TOOLCHAIN "/opt/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/")

# specify the cross compiler
set(CMAKE_C_COMPILER "${PI_TOOLCHAIN}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${PI_TOOLCHAIN}/bin/arm-linux-gnueabihf-g++")

# where is the target environment
set(CMAKE_FIND_ROOT_PATH  "${PI_TOOLCHAIN}")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
