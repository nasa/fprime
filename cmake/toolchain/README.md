# CMake Toolchain files

This directory contains standard toolchain files from CMake. It shows the toolchains supported directly by F prime. This **does not**
 mean other CMake toolchain files will not work, however; other toolchain files will require an F prime platform file created at:

cmake/platform/${CMAKE_SYSTEM_NAME}.cmake

To create a new toolchain, copy the toolchain.template.cmake file and fill it out. Then create a platform file in the platform directory.
Other cmake toolchains (from CMake, other CMake based projects, etc) can be used. The appropriate platform file must be created, if it
does not exist.

See: https://cmake.org/cmake/help/v3.12/manual/cmake-toolchains.7.html
