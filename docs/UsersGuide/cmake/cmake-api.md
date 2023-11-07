## User API Documentation

These links point to documentation needed by most users of the CMake system. The API link
describes the module-level API for working with CMake. This is where users should look for the full
descriptions of module-level functions. Options describe the runtime options that the CMake system
takes. Users wanting to alter the build should look here.

- [API](../api/cmake/API.md): Module API function documentation
- [Options](../api/cmake/Options.md): Runtime build options

An index of all available documentation is available at [CMake API Index](../api/cmake/index.md). This documentation is generated from the source code available [here](https://github.com/nasa/fprime/tree/devel/cmake). Below is a description of some of the main API functionalities.

## Toolchain and Platform Documentation

Toolchain files are used to cross-compile F´. In general, standard CMake toolchain files can be used
to cross-compile, however; F´ includes several built-in toolchains and a template for writing your
own toolchains for F´.

[CMake Toolchain Documentation](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html): CMake's toolchain documentation  
[Toolchain Template](../api/cmake/toolchain/toolchain-template.md): Toolchain file template documentation  
[raspberrypi](../api/cmake/toolchain/raspberrypi.md): Raspberry PI cross-compile toolchain file

Platform files are used to set up F´ for use on specific hardware platforms. This allows users to
define types and headers needed for F´ for any embedded system they desire.

[Platform Template](../api/cmake/platform/platform-template.md): Platform file template documentation  
[Linux](../api/cmake/platform/Linux.md): Linux platform support  
[Darwin](../api/cmake/platform/Darwin.md): Darwin (macOS) platform support  
[rtems5](../api/cmake/platform/rtems5.md): RTEMS 5 initial support  

## Target Documentation

These links document the custom targets integrated into the F´ build system. Targets can be
generically integrated to support both module-level and global targets building toward the same
F´ "target".  For example, the "dict" target may build local dictionary fragments and then roll
them into a global dictionary.

[Target](./Targets.md): target subsystem documentation

All available targets are listed and described in the [CMake API Index](../api/cmake/index.md)


## CMake Support Code Documentation

These links describe the internal CMake function references. These should be consulted when
improvements to the CMake are needed. Build system architects should consult this section.


[FPrime-Code](../api/cmake/FPrime-Code.md): F´ framework code inclusions  
[FPrime](../api/cmake/FPrime.md): F´ project support
