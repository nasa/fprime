**Note:** auto-generated from comments in: ../../support/Executable.cmake

## Executable.cmake:

Contains the code required to make executables and deployments. This is a unified entry point
for any binary that will be built with the F prime CMake system.

Defines the following CMake functions:

generate_executable(EXECUTABLE_NAME AUTOCODER_INPUT_FILES SOURCE_FILES LINK_DEPS)

EXECUTABLE_NAME: name of executable to generate
AUTOCODER_INPUT_FILES: list of autocoder input files to use F prime autocoding
SOURCE_FILES: files used to build the source of this executable. Note: most sources come in
              as link dependencies
LINK_DEPS: non-automatic dependencies needed at link time. Most F prime components and ports
           are found automatically.



## Add "dict" Target:

This target is the target for generating the dictionaries. This will have the output dictionaries registered to
it and make it possible to generate the dictionaries out-of-band from the compile/build.


## Generate Executable:

Top-level executable generation. Core allows for generarion of UT specifics without affecting API.
\param EXECUTABLE_NAME: name of executable to be generated.
\param SOURCE_FILES_INPUT: source files for this executable, split into AC and normal sources
\param DEPS_INPUT: specified module-level dependencies


