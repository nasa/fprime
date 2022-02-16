**Note:** auto-generated from comments in: ./autocoder/ai_xml.cmake

## autocoder/ai-xml.cmake

Primary Ai XML autocoder that sets up the C++ file generation. This is a implementation of the singular autocoder
setup of the the original CMake system.


## `is_supported`:

Given a single input file, determines if that input file is processed by this autocoder. Sets the variable named
IS_SUPPORTED in parent scope to be TRUE if the source is an AI XML file or FALSE otherwise.

AC_INPUT_FILE: filepath for consideration


## `get_generated_files`:

Given a set of supported autocoder input files, this will produce a list of files that will be generated. It sets the
following variables in parent scope:

- GENERATED_FILES: a list of files generated for the given input sources
- MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
- FILE_DEPENDENCIES: specific file dependencies of the given input sources
- EXTRAS: used to publish the 'xml type' file and files to remove

Note: although this function is only required to set `GENERATED_FILES`, the remaining information is also set as
setting this information now will prevent a duplicated call to the tooling.

AC_INPUT_FILES: list of supported autocoder input files


## `get_dependencies`:

Given a set of supported autocoder input files, this will produce a set of dependencies. Since this should have
already been done in `get_generated_files` the implementation just checks the variables are still set.

- MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
- FILE_DEPENDENCIES: specific file dependencies of the given input sources

AC_INPUT_FILES: list of supported autocoder input files


## Function `__ai_info`:

A function used to detect all the needed information for an Ai.xml file. This looks for the following items:
 1. Type of object defined inside: Component, Port, Enum, Serializable, TopologyApp
 2. All fprime module dependencies that may be auto-detected
 3. All file dependencies

- **XML_PATH:** full path to the XML used for sources.
- **MODULE_NAME:** name of the module soliciting new dependencies


## `setup_autocode`:

Sets up the AI XML autocoder to generate files.


