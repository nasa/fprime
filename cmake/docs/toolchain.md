**Note:** auto-generated from comments in: ../toolchain/toolchain.cmake.template

## Template toolchain.cmake.template:

This file acts as a template for the cmake toolchains. These toolchain files
specify what tools to use when performing the build as part of CMake. This
file can be used to quickly set one up.

Follow all the steps in this template to create a toolchain file. Ensure
to remove the template-failsafe (step 1) and fill in all <SOMETHING> tags.

Note: this file should follow the standard CMake toolchain format. See:
https://cmake.org/cmake/help/v3.12/manual/cmake-toolchains.7.html

**Note:** If the user desires to set compile flags, or F prime specific build options, a platform
          file should be constructed. See: [platform.md](platform.md)

### Filling In CMake Toolchain by Eample ###

CMake Toolchain files, at the most basic, define the system name and C and C++ compilers. In
addition, a find path can be set to search for other utilities. This example will walk through
setting these values using the appropriate variables. These can be specified using the following
CMake setting flags:

```
CMAKE_SYSTEM_NAME "RaspberryPI"
# specify the cross compiler
set(CMAKE_C_COMPILER "/opt/rpi/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "/opt/rpi/bin/arm-linux-gnueabihf-g++")
# where is the target environment
set(CMAKE_FIND_ROOT_PATH  "/opt/rpi")
```

**Note:** if copying the template, delete the message with FATAL_ERROR line. This is a fail-safe
          to prevent a raw-copy from being treated as a valid toolchain file.


