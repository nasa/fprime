**Note:** auto-generated from comments in: ../../support/Executable.cmake

## Executable.cmake:

Contains the code required to make executables and deployments. This is a unified entry point
for any binary that will be built with the F prime CMake system.



## Function `add_dict_target`:

This target is the target for generating the dictionaries. This will have the output dictionaries registered to
it and make it possible to generate the dictionaries out-of-band from the compile/build.



## Function `generate_executable:`

Top-level executable generation. Core allows for generarion of UT specifics without affecting API.

- **EXECUTABLE_NAME:** name of executable to be generated.
- **SOURCE_FILES_INPUT:** source files for this executable, split into AC and normal sources
- **DEPS_INPUT:** specified module-level dependencies


