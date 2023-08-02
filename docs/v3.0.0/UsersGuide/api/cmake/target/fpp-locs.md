**Note:** auto-generated from comments in: ./target/fpp-locs.cmake

## fpp-locs.cmake:

fpp-locs is a special target used to build fpp-locs file output. It is run in a special build of the fprime system
specifically focused on generating the locator file for fpp.  It registers normal targets for building these items,
but also comes with a function to run the separate build of fprime.


## Run the CMake build for generating the locs.fpp file. Should be called by the primary build in order to build the
locator files needed by the project.


## fpp-locs function `add_deployment_target`:

Does nothing.  Fpp locations are truly global.


## fpp-locs function `add_global_target`:

This function takes the INPUTS property added to our global


## fpp-locs function `add_module_target`:

This target appends sources that are handled by the fpp autocoder to the source list of the fpp-locs-gen target. This
target will run the location generation from those matching sources

- **MODULE:** name of the module
- **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
- **SOURCE_FILES:** list of source file inputs from the CMakeList.txt setup
- **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt


