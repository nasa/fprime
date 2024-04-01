**Note:** auto-generated from comments in: ./target/fpp_locs.cmake

## fpp_locs.cmake:

fpp_locs is a special target used to build fpp_locs file output. It is run in a special build of the fprime system
specifically focused on generating the locator file for fpp.  It registers normal targets for building these items,
but also comes with a function to run the separate build of fprime.


## Function `determine_global_fpps`:

Processes the global set of modules and determines all FPP files.
OUTPUT_VAR: variable storing the output of this call
MODULES: global set of modules passed in


## Function `generate_locations`:

Generates the FPP locations index. This is needed for all subsequent steps of FPP.


## Function `generate_dependencies`:

Generate dependencies for FPP modules. This is done here for performance, and the generated caches will be read later.
MODULES: modules to generate from


## Function `add_global_target`:

Performs special FPP setup and handling.


## Not defined to prevent defaults from engaging


## Not defined to prevent defaults from engaging


