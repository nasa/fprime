**Note:** auto-generated from comments in: ./utilities.cmake

## utilities.cmake:

Utility and support functions for the fprime CMake build system.


## Function `plugin_name`:

From a plugin include path retrieve the plugin name. This is the name without any .cmake extension.

INCLUDE_PATH: path to plugin
OUTPUT_VARIABLE: variable to set in caller's scope with result


## Function `generate_individual_function_call`:

Generates a routing table entry for the faux cmake_language call for an individual function. This call consists of
a single `elseif(name == function and ARGC == ARG_COUNT)` to support a call to the function with ARG_COUNT arguments.
This is a helper function intended for use within `generate_faux_cmake_language`.

OUTPUT_FILE: file to write these `elseif` blocks into
FUNCTION: name of function to write out
ARG_COUNT: number of args for this particular invocation of the call


## Function `generate_faux_cmake_language`:

This function is used to setup a fake implementation of `cmake_language` calls on implementations of CMake that
predate its creation.  The facsimile is incomplete, but for the purposes of this build system, it will be sufficient
meaning that it can route all the plugin functions correctly but specifically **not** arbitrary function calls.

Functions supported by this call are expected in the GLOBAL property: CMAKE_LANGUAGE_ROUTE_LIST

This is accomplished by writing out a CMake file that contains a macro that looks like the `cmake_language(CALL)`
feature but is implemented by an `if (NAME == FUNCTION) FUNCTION() endif()` table. This file is built within and
included when finished.

In terms of performance:
  - Native `cmake_language(CALL)` is incredibly fast
  - This faux implementation is slow
  - Repetitive including of .cmake files to "switch" implementations (as done in fprime v3.0.0) is **much** slower


## Function `plugin_include_helper`:

Designed to help include API files (targets, autocoders) in an efficient way within CMake. This function imports a
CMake file and defines a `dispatch_<function>(PLUGIN_NAME ...)` function for each function name in ARGN. Thus users
of the imported plugin can call `dispatch_<function>(PLUGIN_NAME ...)` to dispatch a function as implemented in a
plugin.

OUTPUT_VARIABLE: set with the plugin name that has last been included
INCLUDE_PATH: path to file to include


## starts_with:

Check if the string input starts with the given prefix. Sets OUTPUT_VAR to TRUE when it does and sets OUTPUT_VAR to
FALSE when it does not. OUTPUT_VAR is the name of the variable in PARENT_SCOPE that will be set.

Note: regexs in CMake are known to be inefficient. Thus `starts_with` and `ends_with` are implemented without them
in order to ensure speed.

OUTPUT_VAR: variable to set
STRING: string to check
PREFIX: expected ending


## ends_with:

Check if the string input ends with the given suffix. Sets OUTPUT_VAR to TRUE when it does and  sets OUTPUT_VAR to
FALSE when it does not. OUTPUT_VAR is the name of the variable in PARENT_SCOPE that will be set.

Note: regexs in CMake are known to be inefficient. Thus `starts_with` and `ends_with` are implemented without them
in order to ensure speed.

OUTPUT_VAR: variable to set
STRING: string to check
SUFFIX: expected ending


## init_variables:

Initialize all variables passed in to empty variables in the calling scope.


## normalize_paths:

Take in any number of lists of paths and normalize the paths returning a single list.
OUTPUT_NAME: name of variable to set in parent scope


## resolve_dependencies:

Sets OUTPUT_VAR in parent scope to be the set of dependencies in canonical form: relative path from root replacing
directory separators with "_".  E.g. fprime/Fw/Time becomes Fw_Time.

OUTPUT_VAR: variable to fill in parent scope
ARGN: list of dependencies to resolve


## Function `is_target_real`:

Does this target represent a real item (executable, library)? OUTPUT is set to TRUE when real, and FALSE otherwise.

OUTPUT: variable to set
TEST_TARGET: target to set


## Function `is_target_library`:

Does this target represent a real library? OUTPUT is set to TRUE when real, and FALSE otherwise.

OUTPUT: variable to set
TEST_TARGET: target to set


## linker_only:

Checks if a given dependency should be supplied to the linker only. These will not be supplied as CMake dependencies
but will be supplied as link libraries. These tokens are of several types:

1. Linker flags: starts with -l
2. Existing Files: accounts for preexisting libraries shared and otherwise

OUTPUT_VAR: variable to set in PARENT_SCOPE to TRUE/FALSE
TOKEN: token to check if "linker only"


## build_relative_path:

Calculate the path to an item relative to known build paths.  Search is performed in the following order erring if the
item is found in multiple paths.

INPUT_PATH: input path to search
OUTPUT_VAR: output variable to fill


## on_any_changed:

Sets VARIABLE to true if any file has been noted as changed from the "on_changed" function.  Will create cache files
in the binary directory.  Please see: on_changed

INPUT_FILES: files to check for changes
ARGN: passed into execute_process via on_changed call


## on_changed:

Sets VARIABLE to true if and only if the given file has changed since the last time this function was invoked. It will
create "${INPUT_FILE}.prev" in the binary directory as a cache from the previous invocation. The result is always TRUE
unless a successful no-difference is calculated.

INPUT_FILE: file to check if it has changed
ARGN: passed into execute_process


## read_from_lines:

Reads a set of variables from a newline delimited test base. This will read each variable as a separate line. It is
based on the number of arguments passed in.


## Function `full_path_from_build_relative_path`:

Creates a full path from the shortened build-relative path.
-**SHORT_PATH:** build relative path
Return: full path from relative path


## Function `get_nearest_build_root`:

Finds the nearest build root from ${FPRIME_BUILD_LOCATIONS} that is a parent of DIRECTORY_PATH.

- **DIRECTORY_PATH:** path to detect nearest build root
Return: nearest parent from ${FPRIME_BUILD_LOCATIONS}


## Function `get_module_name`:

Takes a path, or something path-like and returns the module's name. This breaks down as the
following:

 1. If passed a path, the module name is the '_'ed variant of the relative path from BUILD_ROOT
 2. If passes something which does not exist on the file system, it is just '_'ed

i.e. ${BUILD_ROOT}/Svc/ActiveLogger becomes Svc_ActiveLogger
     Svc/ActiveLogger also becomes Svc_ActiveLogger

- **DIRECTORY_PATH:** (optional) path to infer MODULE_NAME from. Default: CMAKE_CURRENT_LIST_DIR
- **Return: MODULE_NAME** (set in parent scope)


## Function `get_expected_tool_version`:

Gets the expected tool version named using version identifier VID to name the tools package
file. This will be returned via the variable supplied in FILL_VARIABLE setting it in PARENT_SCOPE.


## Function `set_assert_flags`:

Adds a -DASSERT_FILE_ID=(First 8 digits of MD5) to each source file, and records the output in
hashes.txt. This allows for asserts on file ID not string. Also adds the -DASSERT_RELATIVE_PATH
flag for handling relative path asserts.


## Function `print_property`:

Prints a given property for the module.
- **TARGET**: target to print properties
- **PROPERTY**: name of property to print


## Function `introspect`:

Prints the dependency list of the module supplied as well as the include directories.

- **MODULE_NAME**: module name to print dependencies


