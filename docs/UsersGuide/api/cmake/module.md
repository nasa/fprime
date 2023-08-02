**Note:** auto-generated from comments in: ./module.cmake

## Module.cmake:

This cmake file contains the functions needed to compile a module for F prime. This
includes code for generating Enums, Serializables, Ports, Components, and Topologies.

These are used as the building blocks of F prime items. This includes deployments,
tools, and individual components.


## Function `generate_base_module_properties`:

Helper used to generate the base module properties in the system along with the core target that can be adjusted
later.
- **TARGET_NAME**: target name being generated
- **SOURCE_FILES**: source files as defined by user, unfiltered. Includes autocode and source inputs.
- **DEPENDENCIES**: dependencies as defined by user, unfiltered. Includes target names and link flags.


## Function `generate_deployment:`

Top-level executable generation. Core allows for generation of UT specifics without affecting API.

- **EXECUTABLE_NAME:** name of executable to be generated.
- **SOURCE_FILES:** source files for this executable, split into AC and normal sources
- **DEPENDENCIES:** specified module-level dependencies


## Function `generate_executable:`

Top-level executable generation. Core allows for generation of UT specifics without affecting API.

- **EXECUTABLE_NAME:** name of executable to be generated.
- **SOURCE_FILES:** source files for this executable, split into AC and normal sources
- **DEPENDENCIES:** specified module-level dependencies


## Function `generate_library`:

Generates a library as part of F prime. This runs the AC and all the other items for the build.
It takes SOURCE_FILES_INPUT and DEPS_INPUT, splits them up into ac sources, sources, mod deps,
and library deps.
- *MODULE_NAME:* module name of library to build
- *SOURCE_FILES:* source files that will be split into AC and normal sources.
- *DEPENDENCIES:* dependencies bound for link and cmake dependencies



## Function `generate_ut`:

Generates a unit test as part of F prime. This runs the AC and all the other items for the build.
It takes SOURCE_FILES_INPUT and DEPS_INPUT, splits them up into ac sources, sources, mod deps,
and library deps.
- *UT_EXE_NAME:* exe name of unit test to build
- *UT_SOURCES_FILE:* source files that will be split into AC and normal sources.
- *DEPENDENCIES:* dependencies bound for link and cmake dependencies



