**Note:** auto-generated from comments in: ./toolchain/helpers/arm-linux-base.cmake

## ARM Linux Toolchain Base:

This file provides the basic work for ARM toolchains running on Linux systems. It uses the ARM_TOOL_PREFIX variable to
determine the names of the tools to search for. This variable must be set in the calling script. This toolchain will
find the ARM tools under the path  specified with -DARM_TOOLS_PATH=... and if -DCMAKE_SYSROOT=... is specified then
this path will be used for searching for libraries/headers to compile against.


