**Note:** auto-generated from comments in: ./target/fpp_locs.cmake

## fpp_locs.cmake:

fpp_locs is a special target used to build fpp_locs file output. It is run as part of the sub-build that generates
cached-information about the build itself. This file defines the following target functions:

fpp_locs_add_global_target: global registration target setting up the fpp-locs target run
fpp_locs_add_deployment_target: unused, required for the API
fpp_locs_add_module_target: used to identify all source files to pass to location global target


## Function `fpp_locs_add_global_target`:

Sets up the `fpp_locs` target used to generate the FPP locs file. This is build and then updated in the outer build
cache.
- **TARGET:** name of the target to setup (fpp_locs)


## Function `fpp_locs_add_module_target`:

Pass-through to fpp_locs_add_module_target. FULL_DEPENDENCIES is unused.


## Function `fpp_locs_add_module_target`:

Sets up the list of FPP files used in locations generation.  Each FPP source file for each module is added to the
global target's GLOBAL_FPP_FILES property that is referenced to pass in targets.
- **MODULE:** module name, unused
- **TARGET:** name of the target to setup (fpp_locs)
- **SOURCES:** list of sources filtered to .fpp
- **DEPENDENCIES:** module dependencies, unused.


