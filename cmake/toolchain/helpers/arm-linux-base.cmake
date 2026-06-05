####
# ARM Linux Toolchain Base:
#
# This file provides the basic work for ARM toolchains running on Linux systems. It uses the ARM_TOOL_PREFIX variable to
# determine the names of the tools to search for. This variable must be set in the calling script. This toolchain will
# find the ARM tools under the path  specified with -DARM_TOOLS_PATH=... and if -DCMAKE_SYSROOT=... is specified then
# this path will be used for searching for libraries/headers to compile against.
####
# Set the system information
set(CMAKE_SYSTEM_NAME       Linux CACHE INTERNAL "Set the system name to Linux for this toolchain" FORCE)
set(FPRIME_PLATFORM         Linux CACHE INTERNAL "Set the FPrime platform to Linux for this toolchain" FORCE)
set(CMAKE_SYSTEM_VERSION    0.2 CACHE INTERNAL "Set the system version to 0.2 for this toolchain" FORCE)

# Check ARM tools path
set(FPRIME__INTERNAL_FIND_INPUTS PATHS ENV ARM_TOOLS_PATH PATH_SUFFIXES bin REQUIRED CACHE INTERNAL "Internal find_program inputs for ARM tools" FORCE)
set(FPRIME__INTERNAL_PREFIX_1 "${CMAKE_SYSTEM_PROCESSOR}-linux-gnu${ARM_TOOL_SUFFIX}" CACHE INTERNAL "Internal ARM tool prefix variant 1" FORCE)
set(FPRIME__INTERNAL_PREFIX_2 "${CMAKE_SYSTEM_PROCESSOR}-none-linux-gnu${ARM_TOOL_SUFFIX}" CACHE INTERNAL "Internal ARM tool prefix variant 2" FORCE)
# Set the GNU ARM toolchain
find_program(CMAKE_ASM_COMPILER NAMES ${FPRIME__INTERNAL_PREFIX_1}-as  ${FPRIME__INTERNAL_PREFIX_2}-as  ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_C_COMPILER   NAMES ${FPRIME__INTERNAL_PREFIX_1}-gcc ${FPRIME__INTERNAL_PREFIX_2}-gcc ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_CXX_COMPILER NAMES ${FPRIME__INTERNAL_PREFIX_1}-g++ ${FPRIME__INTERNAL_PREFIX_2}-g++ ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_AR           NAMES ${FPRIME__INTERNAL_PREFIX_1}-ar  ${FPRIME__INTERNAL_PREFIX_2}-ar  ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_OBJCOPY      NAMES ${FPRIME__INTERNAL_PREFIX_1}-objcopy ${FPRIME__INTERNAL_PREFIX_2}-objcopy ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_OBJDUMP      NAMES ${FPRIME__INTERNAL_PREFIX_1}-objdump ${FPRIME__INTERNAL_PREFIX_2}-objdump ${FPRIME__INTERNAL_FIND_INPUTS})

# List programs as found, Note: in toolchain utilities (and message helpers) are not available
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
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE INTERNAL "Set the find root path mode for programs to NEVER for this toolchain" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE INTERNAL "Set the find root path mode for libraries to ONLY for this toolchain" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE INTERNAL "Set the find root path mode for includes to ONLY for this toolchain" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY CACHE INTERNAL "Set the find root path mode for packages to ONLY for this toolchain" FORCE)
