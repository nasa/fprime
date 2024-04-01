# F´ v3.0.0 Migration Guide

This guide is for users hoping to migrate from older F´ versions to F´ v3.0.0.  It will discuss the major differences
between older F´ versions and F´ v3.0.0+ and point out places where users will need to make.

## Modeling Changes

F´v3.0.0 has switched from a baseline using the F´ XML format for modeling F´ constructs to using FPP as a modeling
language. Users may continue to use XML to model components but the F´ team recommends converting to the FPP language
as the XML format will not receive feature enhancements.

There is one caveat that users should be aware of: topology models. If a project wishes to use FPP as a modeling
language for a given topology, it must ensure that all components used by that topology are also modeled with FPP. XML
topologies can include both components modeled in XML and FPP so that projects can begin to migrate to FPP models
without needing to convert everything.

`fpp-from-xml` is a tool included with the FPP tool suit to help convert from XML to FPP.  Further information on the
FPP tool suite and FPP in general can be found at:
[https://fprime-community.github.io/fpp/fpp-users-guide.html](https://fprime-community.github.io/fpp/fpp-users-guide.html)

## Topology Changes

Deployments in older versions of F´ did not treat the Topology as a separate module from the deployment's executable and
entry point code. This setup is limiting and a clear separation of the Topology module and the executable is now required.

In addition, deployments have been split from general executables and should use `register_fprime_deployment()`.

This can be done with the following changes to the CMake setup:

1. In `<Deployment>/Top/CMakeLists.txt` remove the entrypoint code (e.g. `Main.cpp`) and call `register_fprime_module()`
instead of `register_fprime_executable()`.

These modifications can be seen in the Ref app: 
[Ref/Top/CMakeLists.txt](https://github.com/nasa/fprime/blob/034216bc73ac91e78ba03fda25362050a695a960/Ref/Top/CMakeLists.txt#L21)

**Note:** The reference example also show changes necessary to run an FPP topology as discussed above.

2. In `<Deployment>/CMakeLists.txt` include entrypoint code in `SOURCE_FILES` list, set `MOD_DEPS` to include your 
topology module and call `register_fprime_deployment` to setup the executable.
   
These modifications to Ref can be seen here: 
[Ref/CMakeLists.txt](https://github.com/nasa/fprime/blob/034216bc73ac91e78ba03fda25362050a695a960/Ref/CMakeLists.txt#L50-L53)

## Inline Enumerations and Standard Enumerations

Inline enumerations within a Component are no longer supported. F´ supports enumeration models so projects should
migrate to defining an enumeration model and using that type as arguments to commands, events, etc. As part of this work
standard enumerations that were previously implemented as inline enumerations have now been converted. This means
projects must update the usage of these enumerations. In some cases, these enumeration names have been slightly changed.

The best example of this is the command response enumeration.  This change can be seen in the SignalGen component as
shown below.

**Out-of-Date Version 2 Code**
```c++
this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
```
[source](https://github.com/nasa/fprime/blob/ab6b8ade39967843e256841bffabb43129e290d8/Ref/SignalGen/SignalGen.cpp#L167)

**Corrected Version 3 Code**
```c++
this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
```
[source](https://github.com/nasa/fprime/blob/release/v3.0.0/Ref/SignalGen/SignalGen.cpp)

The following user-facing enumeration types must be updated.

| Old Symbol | Updated Symbol  |
|---|---|
| Fw::COMMAND_OK | Fw::CmdResponse::OK |
| Fw::COMMAND_INVALID_OPCODE | Fw::CmdResponse::INVALID_OPCODE |
| Fw::COMMAND_VALIDATION_ERROR | Fw::CmdResponse::VALIDATION_ERROR |
| Fw::COMMAND_FORMAT_ERROR | Fw::CmdResponse::FORMAT_ERROR |
| Fw::COMMAND_EXECUTION_ERROR | Fw::CmdResponse::EXECUTION_ERROR |
| Fw::COMMAND_BUSY | Fw::CmdResponse::BUSY |
| Fw::PARAM_UNINIT | Fw::ParamValid::UNINIT |
| Fw::PARAM_VALID | Fw::ParamValid::VALID |
| Fw::PARAM_INVALID | Fw::ParamValid::INVALID |
| Fw::PARAM_DEFAULT | Fw::ParamValid::DEFAULT |

Other enumerations have changed but these are typically wrapped in the autocoded layer. A complete list is found 
[here](../dev/v3-renamed-symbols.md).

## CMake Options

F´ v3.0.0 has reworked the command line options to `fprime-util generate` to ensure that F´ is in line with CMake
standard usages for CMake variables. In general, F´ no longer depends on certain settings to standard CMake variables.
There are a few exceptions described below.

1. `CMAKE_BUILD_TYPE` is no longer used to specify unit test builds and the user may now override it. `CMAKE_BUILD_TYPE`
may no longer be used to detect if the current build is generating unit tests.
2. `BUILD_TESTING` is now set to `ON` or `OFF` to turn on or off unit test builds. Projects needing to detect unit test 
builds should use `if (BUILD_TESTING)`. This is controlled with the `--ut` switch through `fprime-util`.
3. `CMAKE_INSTALL_PREFIX` is now used to set the output location of the build process (e.g. `build-artifacts`). This was
done to fall in line with the standard CMake installation patterns.

Most users need to set these variables directly when standard build setups are desired as `fprime-util` will set these
variables where needed. However, users needing custom setups may now set `CMAKE_INSTALL_PREFIX` and `CMAKE_BUILD_TYPE`.

## `Os::Task` API Changes

FPP topologies will automatically generate the boilerplate code for starting active components. However, users who start
their own `Os::Task` objects and users who run XML topologies and thus start their own tasks should be aware of changes
to the `Os::Task::start` and `ActiveComponent::start` API.

Arguments to these functions are now unsigned integers in the order: priority, stack size, cpu affinity, and identifier.
Each parameter comes with a default, so none need be specified.  When specified on Posix systems, the OS will attempt to
see these values as requested. However, when permissions are not available to set these the system will fallback to the
default `pthreads` settings.

Calling the older `Os::Task::start` API will result in a compile-time error.

An example of reworking a task start call is seen at: 
[https://github.com/nasa/fprime/blob/ab6b8ade39967843e256841bffabb43129e290d8/Drv/Ip/SocketReadTask.cpp#L35](https://github.com/nasa/fprime/blob/ab6b8ade39967843e256841bffabb43129e290d8/Drv/Ip/SocketReadTask.cpp#L35)

More information is available at:
[https://github.com/nasa/fprime/discussions/1041](https://github.com/nasa/fprime/discussions/1041)