## User API Documentation

These links point to documentation needed by average users of the CMake system. The API link
describes the module-level API for working with CMake. This is where users should look for the full
descriptions of module-level functions. Options describe the runtime options that the CMake system
takes. Users wanting to alter the build should look here.

- [Options](../api/cmake/Options.md): Runtime build options
- [API](../api/cmake/API.md): Module API function documentation

## Toolchain and Platform Documentation

Toolchain files are used to cross-compile F´. In general, standard CMake toolchain files can be used
to cross-compile, however; F´ includes several built-in toolchains and a template for writing your
own toolchains for F´.

[Toolchain Documentation](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html): CMake's toolchain documentation
[Toolchain Template](./toolchain/toolchain-template.md): Toolchain file template documentation
[raspberrypi](./toolchain/raspberrypi.md): Raspberry PI cross-compile toolchain file

Platform files are used to set up F´ for use on specific hardware platforms. This allows users to
define types and headers needed for F´ for any embedded system they desire.

[Platform Template](../api/cmake/platform/platform-template.md): Platform file template documentation
[Linux](../api/cmake/platform/Linux.md): Linux platform support
[Darwin](../api/cmake/platform/Darwin.md): Darwin (macOS) platform support
[rtems5](../api/cmake/platform/rtems5.md): RTEMS 5 initial support
[CMakeLists.txt](./platform/CMakeLists.txt.md)

## Target Documentation

These links document the custom targets integrated into the F´ build system. Targets can be
generically integrated to support both module-level and global targets building toward the same
F´ "target".  For example, the "dict" target may build local dictionary fragments and then roll
them into a global dictionary.

[Target](../api/cmake/support/Target.md): target subsystem documentation
[dict](../api/cmake/target/dict.md): `dict` target documentation
[impl](../api/cmake/target/impl.md): `impl` target documentation
[testimpl](../api/cmake/target/testimpl.md): `testimpl` target documentation
[coverage](../api/cmake/target/coverage.md): `coverage` target documentation


## CMake Support Code Documentation

These links describe the internal CMake function references. These should be consulted when
improvements to the CMake are needed. Build system architects should consult this section.


[FPrime Code](../api/cmake/FPrime-Code.md): F´ framework code inclusions
[FPrime](../api/cmake/FPrime.md): F´ project support
[Module](../api/cmake/support/Module.md): Module registry and autocode support
[Executable](../api/cmake/support/Executable.md): Executable registry and autocoding support
[Unit Test](../api/cmake/support/Unit_Test.md): Unit test registry and autocode support
[Utils](../api/cmake/support/Utils.md): General utility functions
[AC_Utils](../api/cmake/support/AC_Utils.md): Autocode utility implementation specific to the Python autocoder
[Validation](../api/cmake/support/validation/Validation.md): Experimental validation support