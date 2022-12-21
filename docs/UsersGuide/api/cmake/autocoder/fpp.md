**Note:** auto-generated from comments in: ./autocoder/fpp.cmake

## autocoder/fpp.cmake:

CMake implementation of an fprime autocoder. Includes the necessary function definitions to implement the fprime
autocoder API and wraps calls to the FPP tools.


## locate_fpp_tools:

Locates the fpp tool suite and sets FPP_FOUND if the right version of the tools is found. It will look first to the
above install location and then to the system path as a fallback.


## Function `is_supported`:

Required function, processes ComponentAi.xml files.
`AC_INPUT_FILE` potential input to the autocoder


## Function `fpp_get_framework_dependency_helper`:

Helps detect framework dependencies. Either, it calculates specific dependencies *or* if the Fw roll-up target exists,
it will depend on that.  Note: targets within Fw always calculate the internal Fw targets as depending on Fw would
cause a circular dependency.

MODULE_NAME: current module being processed
FRAMEWORK: list of framework dependencies. **NOTE:** will be overridden in PARENT_SCOPE with updated list


## Function `fpp_info`:

Given a set of supported autocoder input files, this will produce a list of files that will be generated. It sets the
following variables in parent scope:

- GENERATED_FILES: a list of files generated for the given input sources
- MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
- FILE_DEPENDENCIES: specific file dependencies of the given input sources
- FPP_IMPORTS: The fpp model dependencies, which end up being the input to the -i flag for the fpp-to-cpp and fpp-to-xml tools

Note: although this function is only required to set `GENERATED_FILES`, the remaining information is also set as
setting this information now will prevent a duplicated call to the tooling.

AC_INPUT_FILES: list of supported autocoder input files


## Function `fpp_setup_autocode`:

Sets up the steps to run the autocoder and produce the files during the build. This is passed the lists generated
in calls to `get_generated_files` and `get_dependencies`.

AC_INPUT_FILES: list of supported autocoder input files


## `fpp_to_modules`:

Helper function. Converts a list of files and a list of autocoder inputs into a list of module names.

FILE_LIST: list of files
OUTPUT_VAR: output variable to set with result


