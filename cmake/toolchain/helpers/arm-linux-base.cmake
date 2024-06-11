####
# ARM Linux Toolchain Base:
#
# This file provides the basic work for ARM toolchains running on Linux systems. It uses the ARM_TOOL_PREFIX variable to
# determine the names of the tools to search for. This variable must be set in the calling script. This toolchain will
# find the ARM tools under the path  specified with -DARM_TOOLS_PATH=... and if -DCMAKE_SYSROOT=... is specified then
# this path will be used for searching for libraries/headers to compile against.
####
# Set the system information
set(CMAKE_SYSTEM_NAME       Linux)
set(CMAKE_SYSTEM_VERSION    0.2)

# Check ARM tools path
set(FIND_INPUTS PATHS ENV ARM_TOOLS_PATH PATH_SUFFIXES bin REQUIRED)
set(PREFIX_1 "${CMAKE_SYSTEM_PROCESSOR}-linux-gnu${ARM_TOOL_SUFFIX}")
set(PREFIX_2 "${CMAKE_SYSTEM_PROCESSOR}-none-linux-gnu${ARM_TOOL_SUFFIX}")
# Set the GNU ARM toolchain
find_program(CMAKE_ASM_COMPILER NAMES ${PREFIX_1}-as  ${PREFIX_2}-as  ${FIND_INPUTS})
find_program(CMAKE_C_COMPILER   NAMES ${PREFIX_1}-gcc ${PREFIX_2}-gcc ${FIND_INPUTS})
find_program(CMAKE_CXX_COMPILER NAMES ${PREFIX_1}-g++ ${PREFIX_2}-g++ ${FIND_INPUTS})
find_program(CMAKE_AR           NAMES ${PREFIX_1}-ar  ${PREFIX_2}-ar  ${FIND_INPUTS})
find_program(CMAKE_OBJCOPY      NAMES ${PREFIX_1}-objcopy ${PREFIX_2}-objcopy ${FIND_INPUTS})
find_program(CMAKE_OBJDUMP      NAMES ${PREFIX_1}-objdump ${PREFIX_2}-objdump ${FIND_INPUTS})

# List programs as found
if (CMAKE_DEBUG_OUTPUT)
    message(STATUS "[arm-linux] Assembler:    ${CMAKE_ASM_COMPILER}")
    message(STATUS "[arm-linux] C   Compiler: ${CMAKE_C_COMPILER}")
    message(STATUS "[arm-linux] CXX Compiler: ${CMAKE_CXX_COMPILER}")
endif()

# Force sysroot onto
if (DEFINED CMAKE_SYSROOT)
    message(STATUS "sysroot set to: ${CMAKE_SYSROOT}")
    set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} "${CMAKE_SYSROOT}")
endif()

# Configure the find commands for finding the toolchain
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
