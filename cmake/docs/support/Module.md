**Note:** auto-generated from comments in: ../../support/Module.cmake

## Module.cmake:

This cmake file contains the functions needed to compile a module for F prime. This
includes code for generating Enums, Serializables, Ports, Components, and Topologies.

These are used as the building blocks of F prime items. This includes deployments,
tools, and indiviual components.


## Autocoder:

This function controls the the generation of the auto-coded filesi, generically, for serializables, ports,
and components. It then mines the XML files for dependencies and then adds them as dependencies to the
module being built.

\param MODULE_NAME: name of the module which is being auto-coded.
\param AUTOCODER_INPUT_FILES: list of input files sent to the autocoder
\param AC_TYPE: type of the auto-coder being invoked,


## Generate Module:

Generates the module as an F prime module. This means that it will process autocoding,
and dependencies. Hard sources are not added here, but in the caller. This will all be
generated into a library.

MOD_DEPS are F prime dependencies.
LINK_DEPS are noraml link dependencies. i.e. -lm


## Generate Library:

Generates a library as part of F prime. This runs the AC and all the other items for the build.
It takes SOURCE_FILES_INPUT and DEPS_INPUT, splits them up into ac sources, sources, mod deps,
and library deps.


