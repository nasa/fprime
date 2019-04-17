**Note:** auto-generated from comments in: ../../support/Module.cmake

## Module.cmake:

This cmake file contains the functions needed to compile a module for F prime. This
includes code for generating Enums, Serializables, Ports, Components, and Topologies.

These are used as the building blocks of F prime items. This includes deployments,
tools, and indiviual components.


## Function `generic_autocoder`:

This function controls the the generation of the auto-coded files, generically, for serializables, ports,
and components. It then mines the XML files for dependencies and then adds them as dependencies to the
module being built.

- **MODULE_NAME:** name of the module which is being auto-coded.
- **AUTOCODER_INPUT_FILES:** list of input files sent to the autocoder
- **AC_TYPE:** type of the auto-coder being invoked,



## Function `generate_module`:

Generates the module as an F prime module. This means that it will process autocoding,
and dependencies. Hard sources are not added here, but in the caller. This will all be
generated into a library.

- **OBJ_NAME:** object name to add dependencies to.
- **AUTOCODER_INPUT_FILES:** files to pass to the autocoder
- **LINK_DEPS:** link-time dependecies like -lm or -lpthread
- **MOD_DEPS:** CMake module dependencies



## Function `generate_library`:

Generates a library as part of F prime. This runs the AC and all the other items for the build.
It takes SOURCE_FILES_INPUT and DEPS_INPUT, splits them up into ac sources, sources, mod deps,
and library deps.

- *SOURCE_FILES_INPUT:* source files that will be split into AC and normal sources.
- *DEPS_INPUT:* dependencies bound for link and cmake dependencies



