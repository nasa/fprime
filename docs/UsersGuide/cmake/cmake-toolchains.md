# CMake Toolchain Files

CMake defines the location, arguments, and properties of build tools using toolchain files. In short, these files
specify what configuration of tools to use to build the CMake project. Since CMake toolchain files can be shared between
projects, F´ mostly uses them as-is. To set platform-specific fprime settings a parallel Platform file may be created in order to keep those settings out of this more generic file.

See: [fprime Platform Files](./cmake-platforms.md)
See: [https://cmake.org/cmake/help/v3.12/manual/cmake-toolchains.7.html](https://cmake.org/cmake/help/v3.12/manual/cmake-toolchains.7.html)

To create a new toolchain, copy the toolchain.template.cmake file and fill it out. Then create a platform file in the
platform directory. Other CMake toolchains (from CMake, other CMake-based projects, etc) can be used. The appropriate
platform file must be created if it does not exist.

To understand the toolchain template: [Toolchain Template File](../api/cmake/toolchain/toolchain-template.md)
To use the template: [fprime Toolchain Template](https://github.com/nasa/fprime/blob/devel/cmake/toolchain/toolchain.cmake.template)