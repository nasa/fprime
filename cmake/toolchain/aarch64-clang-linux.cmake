####
# ARM 64-bit Clang/LLVM Toolchain
#
# Cross-compiles for 64-bit ARM (aarch64) Linux targets (e.g. Raspberry Pi 5) using a generic
# clang/lld installation (e.g. an llvm.org release, Apple clang + lld, or a distribution clang)
# together with a target sysroot. Since clang is a native cross-compiler, the same toolchain file
# works on Linux and macOS hosts.
#
# Requirements:
#  1. clang, clang++, and lld on the PATH, or their root specified with -DLLVM_TOOLS_PATH=... (or
#     the LLVM_TOOLS_PATH environment variable) pointing at an LLVM installation containing bin/.
#  2. A sysroot for the target (glibc, libstdc++, GCC runtime) specified with -DCMAKE_SYSROOT=...
#     or the AARCH64_SYSROOT environment variable. Such a sysroot may be extracted from an existing
#     GNU cross-toolchain (its `libc` tree plus libstdc++ headers and the lib/gcc runtime) or from
#     the target system itself.
####
set(CMAKE_SYSTEM_NAME       Linux)
set(FPRIME_PLATFORM         Linux)
set(CMAKE_SYSTEM_PROCESSOR  "aarch64")
set(CMAKE_SYSTEM_VERSION    0.2)

set(FPRIME__INTERNAL_TRIPLE "aarch64-none-linux-gnu")
set(FPRIME__INTERNAL_FIND_INPUTS HINTS ${LLVM_TOOLS_PATH} ENV LLVM_TOOLS_PATH PATH_SUFFIXES bin REQUIRED)
# Ensure -DLLVM_TOOLS_PATH=... is visible to try_compile sub-configurations, which re-read this file
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES LLVM_TOOLS_PATH)

find_program(CMAKE_C_COMPILER   NAMES clang       ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_CXX_COMPILER NAMES clang++     ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_ASM_COMPILER NAMES clang       ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_AR           NAMES llvm-ar     ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_RANLIB       NAMES llvm-ranlib ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_OBJCOPY      NAMES llvm-objcopy ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_OBJDUMP      NAMES llvm-objdump ${FPRIME__INTERNAL_FIND_INPUTS})
find_program(CMAKE_LINKER       NAMES ld.lld      ${FPRIME__INTERNAL_FIND_INPUTS})

# Cross-compilation target triple (drives clang's --target flag)
set(CMAKE_C_COMPILER_TARGET   "${FPRIME__INTERNAL_TRIPLE}")
set(CMAKE_CXX_COMPILER_TARGET "${FPRIME__INTERNAL_TRIPLE}")
set(CMAKE_ASM_COMPILER_TARGET "${FPRIME__INTERNAL_TRIPLE}")

# Allow the sysroot to come from the AARCH64_SYSROOT environment variable
if (NOT DEFINED CMAKE_SYSROOT AND NOT "$ENV{AARCH64_SYSROOT}" STREQUAL "")
    set(CMAKE_SYSROOT "$ENV{AARCH64_SYSROOT}" CACHE PATH "Target sysroot for aarch64 cross-compilation")
endif()
if (NOT DEFINED CMAKE_SYSROOT OR "${CMAKE_SYSROOT}" STREQUAL "")
    message(FATAL_ERROR "A target sysroot is required: set -DCMAKE_SYSROOT=... or the AARCH64_SYSROOT environment variable")
endif()
set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} "${CMAKE_SYSROOT}")

# Use lld, which (unlike the host's default linker on macOS or an x86-only GNU ld) links aarch64 Linux ELF
add_link_options(-fuse-ld=lld)

if (CMAKE_DEBUG_OUTPUT)
    message(STATUS "[aarch64-clang-linux] C   Compiler: ${CMAKE_C_COMPILER}")
    message(STATUS "[aarch64-clang-linux] CXX Compiler: ${CMAKE_CXX_COMPILER}")
    message(STATUS "[aarch64-clang-linux] Sysroot:      ${CMAKE_SYSROOT}")
endif()

# Configure the find commands for finding the toolchain
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
