**Note:** auto-generated from comments in: ./utilities.cmake

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

- **DIRECTORY_PATH:** path to infer MODULE_NAME from
- **Return: MODULE_NAME** (set in parent scope)


## Function `set_hash_flag`:

Adds a -DASSERT_FILE_ID=(First 8 digits of MD5) to each source file, and records the output in
hashes.txt. This allows for asserts on file ID not string.


## Function `print_property`:

Prints a given property for the module.
- **TARGET**: target to print properties
- **PROPERTY**: name of property to print


## Function `introspect`:

Prints the dependency list of the module supplied as well as the include directories.

- **MODULE_NAME**: module name to print dependencies


