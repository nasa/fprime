####
# File: toolchain/generic-native.cmake:
#
# A toolchain file for compiling natively, but using a "Generic" toolchain. This tests that "Generic" toolchains work
# within F Prime.
#
####
# A normal F Prime toolchain would set the CMAKE_SYSTEM_NAME to Generic and set FPRIME_PLATFORM. However, this is a
# customization of the toolchain for fprime.  This toolchain is a generic CMake toolchain where FPRIME_PLATFORM is set
# via a -DFPRIME_PLATFORM flag, showing this other route to use CMake toolchains as-is.
set(CMAKE_SYSTEM_NAME Generic)

# Standard program names
find_program(CMAKE_C_COMPILER   NAMES cc gcc clang)
find_program(CMAKE_CXX_COMPILER NAMES c++ g++ clang++)
