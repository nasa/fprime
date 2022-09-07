set(CMAKE_SYSTEM_NAME "stm")

# Heaps of work here setting compiler flags for arm toolchain !!!!!!!!!!!!!!!
set(CMAKE_C_COMPILER "/usr/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/arm-none-eabi-gcc")

# STEP 4: Specify paths to root of toolchain package, for searching for
#         libraries, executables, etc.
set(CMAKE_FIND_ROOT_PATH  NEVER)

# DO NOT EDIT: F prime searches the host for programs, not the cross
# compile toolchain
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# DO NOT EDIT: F prime searches for libs, includes, and packages in the
# toolchain when cross-compiling.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
