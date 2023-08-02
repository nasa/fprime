**Note:** auto-generated from comments in: ./prescan.cmake

## prescan.cmake:

This file performs a prescan of the fprime source. This prescan looks-up FPP files and writes some other cache files
in order to prepare for running FPP tools before the full source scan and build. This is done because the FPP tools
need to do two items before the full build: setup a locations index, and (for runtime efficiency) generate per-module
dependencies.

This file runs CMake on fprime, but registering only a single target: [prescan](./target/prescan.cmake) that does the
work of setting up the cache and location information. This CMake build is as reduced as possible, turning off
compiler checks, running an individual target, but it **must** inherit the same properties as specified in the outer
full build or the information will be wrong.


## Function `_get_call_properties`:

Gets a list of properties to send to the call based on the values of these properties in the current build. This will
set CALL_PROPS in PARENT_SCOPE and is only intended to be run from within perform_prescan,


## Function `perform_prescan`:

Perform the prescan by internally running CMake. This call needs to be as fast as possible, and should not require
doing any actual building. That is, it just generates, using the generate byproducts as a cache to later in the real
generate and build.


