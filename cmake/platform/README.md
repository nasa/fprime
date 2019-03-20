# F´ and CMake Platforms

In this directory, users can create platform-specific build files for the purposes of tailoring F prime
for given platform targets. Any CMake toolchain file should work, but it will require a platform file
created here to add target specific configuration using the name "${CMAKE_SYSTEM_NAME}.cmake".

At minimum this file can be blank, but more commonly includeds paths for "StandardTypes.hpp" to support.
Build flags, and other includes can be added in to support different compile time options. In addition,
these files can define CMake option flags specific to the build.

In order to create a new platform from scratch, the use can copy "platform.cmake.template" and fill it
out in order to generate the new platform file. It will guide the user through the setup of this
piece.

