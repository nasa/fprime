####
# ARM 64-bit Toolchain
#
# This ARM toolchain will compile for 64-bit ARM systems running linux. It uses the arm packages installed on the system
# path for cross-compilation. To override the location of the tools use -DARM_TOOLS_PATH=... to specify the root
# directory of the tools installation. That directory should contain folders bin, lib, etc where the tools are located.
#
# These toolchains will use the linux libraries shipped with the compiler to build the final image. To override this,
# users should set -DCMAKE_SYSROOT=... to a directory containing a valid sysroot for their device.
#
# Cautions:
#  1. Care must be taken to ensure that the Linux OS running on the target is newer than the cross-compilers, or sysroot
#     should be used to specify fixed library targets to compile against.
#  2. Specifying sysroot should be used with care as both libraries and headers must exist both in the sysroot
#
####
set(CMAKE_SYSTEM_PROCESSOR "aarch64")
include("${CMAKE_CURRENT_LIST_DIR}/helpers/arm-linux-base.cmake")
