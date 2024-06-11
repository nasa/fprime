####
# Raspberry PI Toolchain
#
# This is a toolchain for the Raspberry Pi. This toolchain can be used to build
# against the Raspberry Pi embedded Linux target. In order to use this toolchain,
# the Raspberry Pi cross-compiler should be installed on a Linux host. These
# tools are installable as follows:
#   sudo apt install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf  gdb-multiarch
####

set(CMAKE_SYSTEM_PROCESSOR "arm")
set(ARM_TOOL_SUFFIX eabihf)

if(DEFINED ENV{RPI_TOOLCHAIN_DIR})
    set(ENV{ARM_TOOLS_PATH} "$ENV{RPI_TOOLCHAIN_DIR}")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/helpers/arm-linux-base.cmake")
