**Note:** auto-generated from comments in: ./sub-build/sub-build.cmake

## sub-build.cmake:

CMake occasionally requires sub-builds in order to generate certain types of information.  The sub-build mechanics are
implemented in this file. This includes the ability to setup targets, builds, and handle information passing back and
forth between the modules.

This file runs CMake on fprime, but registering only a set of target: (e.g. [fpp_locs](./target/fpp_locs.cmake) that
do the work on the build. This allows CMake to generate build information to be consumed at the generate phase while
maintaining the efficiency of properly expressed builds.


## Function `run_sub_build`:

Runs a sub-build with the supplied SUB_BUILD_NAME. Output is set in PARENT_SCOPE to the name variable named by OUTPUT.
The targets registered and run as part of the sub-build are supplied in ARGN.

Targets specified in ARGN **must** define top-level global targets in-order for the sub-build target to build
correctly. Defining a no-op global target will result in only the generate step performing any useful actions, which
may, in some cases, be desired.

`SUB_BUILD_NAME`: name of the sub-build and directory to contain its build cache
`JOBS`: number of jobs to use in sub-build
`ARGN`: list of targets to run as part of this sub-build. These will be registered, and run in order.


## Function `_get_call_properties`:

Gets a list of properties to send to the call based on the values of these properties in the current build. This will
set CALL_PROPS in PARENT_SCOPE and is only intended to be run from within the ,


