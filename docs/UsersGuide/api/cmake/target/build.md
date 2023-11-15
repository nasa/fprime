**Note:** auto-generated from comments in: ./target/build.cmake

## build.cmake:

This target sets up the build for every module in the system. WARNING: it registers a target set to the module name,
not including _build. This is for historical reasons.


## Build function `add_global_target`:

Specifically does nothing.  The "all" target of a normal cmake build will cover this case.


## build_setup_build_module:

Helper function to setup the module. This was the historical core of the CMake system, now embedded as part of this
build target. It adds a the target (library, executable), sets up compiler source files, flags generated sources,
sets up module and linker dependencies adds the file to the hashes.txt file, sets up include directories, etc.

- MODULE: module name being setup
- SOURCES: hand-specified source files
- GENERATED: generated sources
- EXCLUDED_SOURCES: sources already "consumed", that is, processed by an autocoder
- DEPENDENCIES: dependencies of this module. Also link flags and libraries.


## Function `add_deployment_target`:

Adds in a deployment target, which for build, is just a normal module target. See: add_module_target for a description
of arguments. FULL_DEPENDENCY_LIST is unused (these are already known to CMake).


## Function `build_add_module_target`:

Adds a module-by-module target for building fprime.

- **MODULE:** name of the module
- **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
- **SOURCES:** list of source file inputs from the CMakeLists.txt setup
- **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt


