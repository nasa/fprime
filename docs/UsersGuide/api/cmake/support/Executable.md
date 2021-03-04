**Note:** auto-generated from comments in: ./support/Executable.cmake

## Executable.cmake:

Contains the code required to make executables and deployments. This is a unified entry point
for any binary that will be built with the F prime CMake system.



## Function `generate_executable:`

Top-level executable generation. Core allows for generation of UT specifics without affecting API.

- **EXECUTABLE_NAME:** name of executable to be generated.
- **SOURCE_FILES_INPUT:** source files for this executable, split into AC and normal sources
- **DEPS_INPUT:** specified module-level dependencies


