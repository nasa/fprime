# F´ and CMake Platforms

Users can create platform-specific build files for the purposes of tailoring fprime
for given platform targets. Any CMake toolchain file should work, but it will require a platform file created here to add target-specific configuration using the name "${CMAKE_SYSTEM_NAME}.cmake".

At minimum, this file can be blank, but more commonly there are included paths for "StandardTypes.hpp" to support.
Build flags and other includes can be added in to support different compile-time options. In addition, these files can define CMake option flags specific to the build.

In order to create a new platform from scratch, the user can copy "platform.cmake.template" and fill it out to generate the new platform file. It will guide the user through the setup of this piece.

To understand the platform template: [Platform Template File](https://github.com/nasa/fprime/blob/release/documentation/docs/UsersGuide/api/cmake/platform/platform-template.md)
To use the template: [fprime Platform Template](https://github.com/nasa/fprime/blob/devel/cmake/platform/platform.cmake.template)
