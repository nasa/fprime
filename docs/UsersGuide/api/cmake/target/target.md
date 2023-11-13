**Note:** auto-generated from comments in: ./target/target.cmake

## Target.cmake:

Functions supporting the F prime target additions. These targets allow building against modules
and top-level targets. This allows for multi-part builds like `sloc` or `dict` where some part
applies to the module and is rolled up into some global command. Target files must define two
functions `add_module_target` and `add_global_target`.

### `add_global_target` Specification:

Adds a global target for the custom target. This handles the top-level target for the build.
Think `make dict`. It is the global root for all of the children module defines. This should call
CMake's `add_custom_target("${TARGET_NAME}" ...)` at some point in the function. Also pass the
`ALL` argument if it should be built as part of a standard build i.e. `make`.

**Arguments passed in:**
 - **TARGET_NAME:** target name to be added. **Must** be passed to a call to `add_custom_target`

### `add_module_target` Specification:

Adds a module-by-module target for this given target. Any subtargets for each module and their
commands should be registered via CMake's `add_custom_target("${TARGET_NAME}" ...)`. This command
is supplied with all of the modules knowledge. Add a `DEPENDS` call on AC_OUTPUTS to come after
the autocoding step.

**Arguments passed in:**
 - **MODULE_NAME:** name of the module being built.
 - **TARGET_NAME:** target name to be added. **Must** be passed to a call to `add_custom_target`
 - **AC_INPUTS:** list of autocoder inputs. These are Ai.xml files
 - **SOURCE_FILES:** list of source file inputs. These are handwritten *.cpp and *.hpp.
 - **AC_OUTPUTS:** list of autocoder outputs. These are Ac.cpp and Ac.hpp files.
 - **MOD_DEPS:** list of specified dependencies of target. Use: fprime_ai_info for Ai.xml info


## Function `get_target_name`:

Gets the target name from the path to the target file. Two variants of this name will be
generated and placed in parent scope: TARGET_NAME, and TARGET_MOD_NAME.

- **MODULE_NAME:** module name for TARGET_MOD_NAME variant
- **Return: TARGET_NAME** (set in parent scope), global target name i.e. `dict`.
- **Return: TARGET_MOD_NAME** (set in parent scope), module target name. i.e. `Fw_Cfg_dict`

**Note:** TARGET_MOD_NAME not set if optional argument `MODULE_NAME` not supplied


## Function `setup_global_targets`:

Loops through all targets registered and sets up the global target.


## Function `setup_global_target`:

Setup a given target file in global scope. This also includes the target file once and thus must be called regardless
of the actual existence of a global entry point for a given target. All targets **must** define a function of the form
${TARGET_NAME}_add_global_target though it may be empty.

TARGET_FILE: target file to include


## Function `setup_single_target`:

Setup a given target file's module-specific targets. There are two module-specific target options. The first is a
normal module target called through ${TARGET_NAME}_add_module_target. This is for setting up items registered through
register_fprime_module calls. The second is called through ${TARGET_NAME}_add_deployment_target and responds to calls
of register_fprime_deployment. Both add_*_target functions must be defined, may be empty implementations. Only one of
the two functions will be called for a given module.

TARGET_FILE: target file to include
MODULE: module being processed
SOURCES: sources specified with `set(SOURCE_FILES ...)` in module's CMakeLists.txt
DEPENDENCIES: dependencies and link libraries specified with `set(MOD_DEPS ...)` in module's CMakeLists.txt


## Function `setup_module_targets`:

Takes all registered targets and sets up the module specific targets from them. The list of targets  is read from the
global property FPRIME_TARGET_LIST.

- MODULE: name of the module being processed
- SOURCES: sources specified with `set(SOURCE_FILES ...)` in module's CMakeLists.txt
- DEPENDENCIES: dependencies and link libraries specified with `set(MOD_DEPS ...)` in module's CMakeLists.txt


## Function `recurse_targets`:

A helper that pulls out module dependencies that are also fprime modules.


## See Also:
 - API: [API](../API.md) describes the `register_fprime_target` function


