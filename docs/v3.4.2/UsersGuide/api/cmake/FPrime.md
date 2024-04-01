**Note:** auto-generated from comments in: ./FPrime.cmake

## FPrime.cmake:

This file is the entry point for building fprime projects, libraries. It does not setup F prime as a project, but
rather allows the users to build against fprime, fprime libraries while taking advantage of fprime's autocoding
support. This file includes the cmake build system setup for building like fprime.


## Function `fprime_setup_global_includes`:

Adds basic include directories that make fprime work. This ensures that configuration, framework, and project all
function as expected. This will also include the internal build-cache directories.


## Function `fprime_detect_libraries`:

This function detects libraries using the FPRIME_LIBRARY_LOCATIONS variable. Fore each library path, the following is
done:
1. Detect a manifest file from in-order: `library.cmake`, and then `<library name>.cmake`
2. Add the library's top-level cmake directory to the CMAKE_MODULE_PATH
3. Add the library root as an include directory
4. Add option() to disable library UTs


## Function `fprime_setup_standard_targets`:

Registers the targets required for a standard fprime build. This will be changed when FPRIME_SUB_BUILD_TARGETS.


## Function `fprime_setup_override_targets`:

Override the targets that are registered by the default build with those supplied in FPRIME_SUB_BUILD_TARGETS. If
FPRIME_SUB_BUILD_TARGETS is defined, nothing happens.


## Function `fprime_setup_included_code`:

Sets up the code/build for fprime and libraries. Call after all project specific targets and autocoders are set up and
registered.


