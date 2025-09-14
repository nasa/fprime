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
set(FPRIME_PLATFORM         Linux)
set(CMAKE_SYSTEM_VERSION    0.2)

# Check ARM tools path
set(FPRIME__INTERNAL_FIND_INPUTS PATHS ENV ARM_TOOLS_PATH PATH_SUFFIXES bin REQUIRED)
set(FPRIME__INTERNAL_PREFIX_1 "${CMAKE_SYSTEM_PROCESSOR}-linux-gnu${ARM_TOOL_SUFFIX}")
set(FPRIME__INTERNAL_PREFIX_2 "${CMAKE_SYSTEM_PROCESSOR}-none-linux-gnu${ARM_TOOL_SUFFIX}")
# Set the GNU ARM toolchain
find_program(CMAKE_ASM_COMPILER NAMES ${FPRIME__INTERNAL_PREFIX_1}-as  ${FPRIME__INTERNAL_PREFIX_2}-as  ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_C_COMPILER   NAMES ${FPRIME__INTERNAL_PREFIX_1}-gcc ${FPRIME__INTERNAL_PREFIX_2}-gcc ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_CXX_COMPILER NAMES ${FPRIME__INTERNAL_PREFIX_1}-g++ ${FPRIME__INTERNAL_PREFIX_2}-g++ ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_AR           NAMES ${FPRIME__INTERNAL_PREFIX_1}-ar  ${FPRIME__INTERNAL_PREFIX_2}-ar  ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_OBJCOPY      NAMES ${FPRIME__INTERNAL_PREFIX_1}-objcopy ${FPRIME__INTERNAL_PREFIX_2}-objcopy ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_OBJDUMP      NAMES ${FPRIME__INTERNAL_PREFIX_1}-objdump ${FPRIME__INTERNAL_PREFIX_2}-objdump ${FPRIME__INTERNAL_FIND_INPUTS})

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
