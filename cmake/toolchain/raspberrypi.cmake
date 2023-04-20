####
# Raspberry PI Toolchain
#
# This is a toolchain for the Raspberry Pi. This toolchain can be used ot build
# against the Raspberry Pi embedded Linux target. In order to use this toolchain,
# the Raspberry Pi cross-compiler should be installed on a Linux host. These
# tools are installable as follows:
#   sudo apt install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf  gdb-multiarch
####

# Set the system information
set(CMAKE_SYSTEM_NAME       Linux)
set(CMAKE_SYSTEM_PROCESSOR  arm)
set(CMAKE_SYSTEM_VERSION    1)
# Configure the find commands for cross-compiling: tools from host, libraries, includes, and packages from cross-compiler/sysroot
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM   NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY   ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE   ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE   ONLY)

# Check CMake sysroot
if (DEFINED CMAKE_SYSROOT)
    set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT} )
endif()

# Set the GNU ARM toolchain
find_program(CMAKE_AR NAMES arm-linux-gnueabihf-ar PATHS ENV RPI_TOOLCHAIN_DIR PATH_SUFFIXES bin REQUIRED)
find_program(CMAKE_C_COMPILER NAMES arm-linux-gnueabihf-gcc PATHS ENV RPI_TOOLCHAIN_DIR PATH_SUFFIXES bin REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES arm-linux-gnueabihf-g++ PATHS ENV RPI_TOOLCHAIN_DIR PATH_SUFFIXES bin REQUIRED)
message(STATUS "[arm-linux] C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "[arm-linux] CXX Compiler: ${CMAKE_CXX_COMPILER}")
find_program(CMAKE_ASM_COMPILER NAMES arm-linux-gnueabihf-as PATHS ENV RPI_TOOLCHAIN_DIR PATH_SUFFIXES bin REQUIRED)
find_program(CMAKE_OBJCOPY NAMES arm-linux-gnueabihf-objcopy PATHS ENV RPI_TOOLCHAIN_DIR PATH_SUFFIXES bin REQUIRED)
find_program(CMAKE_OBJDUMP NAMES arm-linux-gnueabihf-objdump PATHS ENV RPI_TOOLCHAIN_DIR PATH_SUFFIXES bin REQUIRED)

