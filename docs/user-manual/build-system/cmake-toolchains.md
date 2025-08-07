# CMake Toolchain Files

CMake defines the location, arguments, and properties of build tools using toolchain files. In short, these files
specify what configuration of tools to use to build the CMake project. Since CMake toolchain files can be shared between
projects, F´ mostly uses them as-is. To set platform-specific fprime settings a parallel Platform file may be created in order to keep those settings out of this more generic file.


See: [https://cmake.org/cmake/help/v3.12/manual/cmake-toolchains.7.html](https://cmake.org/cmake/help/v3.12/manual/cmake-toolchains.7.html)

CMake toolchains are often adapted from vendor-supplied toolchains, or from one F´ provides. Users should set the variable
`FPRIME_PLATFORM` in their toolchain to specify the F Prime platform file. See: [fprime Platform Files](./cmake-platforms.md)