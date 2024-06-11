####
# Target.cmake:
#
# Functions supporting the F prime target additions. These targets allow building against modules
# and top-level targets. This allows for multi-part builds like `sloc` or `dict` where some part
# applies to the module and is rolled up into some global command. Target files must define two
# functions `add_module_target` and `add_global_target`.
#
# ### `add_global_target` Specification:
#
# Adds a global target for the custom target. This handles the top-level target for the build.
# Think `make dict`. It is the global root for all of the children module defines. This should call
# CMake's `add_custom_target("${TARGET_NAME}" ...)` at some point in the function. Also pass the
# `ALL` argument if it should be built as part of a standard build i.e. `make`.
#
# **Arguments passed in:**
#  - **TARGET_NAME:** target name to be added. **Must** be passed to a call to `add_custom_target`
#
# ### `add_module_target` Specification:
#
# Adds a module-by-module target for this given target. Any subtargets for each module and their
# commands should be registered via CMake's `add_custom_target("${TARGET_NAME}" ...)`. This command
# is supplied with all of the modules knowledge. Add a `DEPENDS` call on AC_OUTPUTS to come after
# the autocoding step.
#
# **Arguments passed in:**
#  - **MODULE_NAME:** name of the module being built.
#  - **TARGET_NAME:** target name to be added. **Must** be passed to a call to `add_custom_target`
#  - **AC_INPUTS:** list of autocoder inputs. These are Ai.xml files
#  - **SOURCE_FILES:** list of source file inputs. These are handwritten *.cpp and *.hpp.
#  - **AC_OUTPUTS:** list of autocoder outputs. These are Ac.cpp and Ac.hpp files.
#  - **MOD_DEPS:** list of specified dependencies of target. Use: fprime_ai_info for Ai.xml info
####
include_guard()

####
# Function `get_target_name`:
#
# Gets the target name from the path to the target file. Two variants of this name will be
# generated and placed in parent scope: TARGET_NAME, and TARGET_MOD_NAME.
#
# - **MODULE_NAME:** module name for TARGET_MOD_NAME variant
# - **Return: TARGET_NAME** (set in parent scope), global target name i.e. `dict`.
# - **Return: TARGET_MOD_NAME** (set in parent scope), module target name. i.e. `Fw_Cfg_dict`
#
# **Note:** TARGET_MOD_NAME not set if optional argument `MODULE_NAME` not supplied
####
function(get_target_name TARGET_FILE_PATH)
    get_filename_component(BASE_VAR ${TARGET_FILE_PATH} NAME_WE)
    set(TARGET_NAME ${BASE_VAR} PARENT_SCOPE)
    if (${ARGC} GREATER 1)
        set(TARGET_MOD_NAME "${ARGV1}_${BASE_VAR}" PARENT_SCOPE)
    endif()
endfunction(get_target_name)

####
# Function `setup_global_targets`:
#
# Loops through all targets registered and sets up the global target.
####
function(setup_global_targets)
    # Get both normal and ut target lists
    get_property(TARGETS GLOBAL PROPERTY FPRIME_TARGET_LIST)
    get_property(UT_TARGETS GLOBAL PROPERTY FPRIME_UT_TARGET_LIST)

    # Register targets
    foreach(TARGET IN LISTS TARGETS)
        setup_global_target("${TARGET}")
    endforeach()
    # Register targets specific to UT build
    if (BUILD_TESTING)
        foreach(TARGET IN LISTS UT_TARGETS)
            setup_global_target("${TARGET}")
        endforeach()
    endif ()
endfunction(setup_global_targets)


####
# Function `setup_global_target`:
#
# Setup a given target file in global scope. This also includes the target file once and thus must be called regardless
# of the actual existence of a global entry point for a given target. All targets **must** define a function of the form
# ${TARGET_NAME}_add_global_target though it may be empty.
#
# TARGET_FILE: target file to include
####
function(setup_global_target TARGET_FILE)
    plugin_include_helper(TARGET_NAME "${TARGET_FILE}" add_global_target add_module_target add_deployment_target)
    cmake_language(CALL "${TARGET_NAME}_add_global_target" "${TARGET_NAME}")
endfunction(setup_global_target)

####
# Function `setup_single_target`:
#
# Setup a given target file's module-specific targets. There are two module-specific target options. The first is a
# normal module target called through ${TARGET_NAME}_add_module_target. This is for setting up items registered through
# register_fprime_module calls. The second is called through ${TARGET_NAME}_add_deployment_target and responds to calls
# of register_fprime_deployment. Both add_*_target functions must be defined, may be empty implementations. Only one of
# the two functions will be called for a given module.
#
# TARGET_FILE: target file to include
# MODULE: module being processed
# SOURCES: sources specified with `set(SOURCE_FILES ...)` in module's CMakeLists.txt
# DEPENDENCIES: dependencies and link libraries specified with `set(MOD_DEPS ...)` in module's CMakeLists.txt
####
function(setup_single_target TARGET_FILE MODULE SOURCES DEPENDENCIES)
    # Announce for the debug log
    get_target_name("${TARGET_FILE}")
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[target] Setting up '${TARGET_NAME}' on all module ${MODULE}")
    endif()
    get_target_property(MODULE_TYPE "${MODULE}" FP_TYPE)

    if (NOT MODULE_TYPE STREQUAL "Deployment")
        cmake_language(CALL "${TARGET_NAME}_add_module_target" "${MODULE}" "${TARGET_NAME}" "${SOURCES}" "${DEPENDENCIES}")
    else()
        get_target_property(RECURSIVE_DEPENDENCIES "${MODULE}" FP_RECURSIVE_DEPS)
        if (NOT RECURSIVE_DEPENDENCIES AND NOT DEFINED FPRIME_PRESCAN)
            resolve_dependencies(RESOLVED ${DEPENDENCIES})
            recurse_targets("${MODULE}" RECURSIVE_DEPENDENCIES "" ${RESOLVED})
            set_target_properties("${MODULE}" PROPERTIES FP_RECURSIVE_DEPS "${RECURSIVE_DEPENDENCIES}")
        endif()
        cmake_language(CALL "${TARGET_NAME}_add_deployment_target" "${MODULE}" "${TARGET_NAME}" "${SOURCES}" "${DEPENDENCIES}" "${RECURSIVE_DEPENDENCIES}")
    endif()
endfunction(setup_single_target)

####
# Function `setup_module_targets`:
#
# Takes all registered targets and sets up the module specific targets from them. The list of targets  is read from the
# global property FPRIME_TARGET_LIST.
#
# - MODULE: name of the module being processed
# - SOURCES: sources specified with `set(SOURCE_FILES ...)` in module's CMakeLists.txt
# - DEPENDENCIES: dependencies and link libraries specified with `set(MOD_DEPS ...)` in module's CMakeLists.txt
####
function(setup_module_targets MODULE SOURCES DEPENDENCIES)
    # Grab the list of targets
    set(LIST_NAME FPRIME_TARGET_LIST)
    get_target_property(MODULE_TYPE "${MODULE}" FP_TYPE)

    # Get both normal and ut target lists
    get_property(TARGETS GLOBAL PROPERTY FPRIME_TARGET_LIST)
    get_property(UT_TARGETS GLOBAL PROPERTY FPRIME_UT_TARGET_LIST)
    # UT targets are the only targets run on unit tests, and are included in deployments
    if (MODULE_TYPE STREQUAL "Deployment")
        list(APPEND TARGETS ${UT_TARGETS})
    elseif (MODULE_TYPE STREQUAL "Unit Test")
        set(TARGETS "${UT_TARGETS}")
    endif()

    # Now run through each of the determined targets
    foreach(TARGET IN LISTS TARGETS)
        setup_single_target("${TARGET}" "${MODULE}" "${SOURCES}" "${DEPENDENCIES}")
    endforeach()
endfunction(setup_module_targets)

####
# Function `recurse_targets`:
#
# A helper that pulls out module dependencies that are also fprime modules.
####
function(recurse_targets TARGET OUTPUT BOUND)
    get_property(ALL_MODULES GLOBAL PROPERTY FPRIME_MODULES)
    set(TARGET_DEPENDENCIES)
    if (TARGET "${TARGET}")
        get_property(TARGET_DEPENDENCIES TARGET "${TARGET}" PROPERTY FPRIME_TARGET_DEPENDENCIES)
    endif()
    # Extra dependencies
    list(APPEND TARGET_DEPENDENCIES ${ARGN})
    if (TARGET_DEPENDENCIES)
        list(REMOVE_DUPLICATES TARGET_DEPENDENCIES)
    endif()

    set(RESULTS_LOCAL)
    foreach(NEW_TARGET IN LISTS TARGET_DEPENDENCIES)
        if (NOT NEW_TARGET IN_LIST BOUND AND NEW_TARGET IN_LIST ALL_MODULES)
            list(APPEND BOUND "${NEW_TARGET}")
            recurse_targets("${NEW_TARGET}" RESULTS "${BOUND}")
            list(APPEND RESULTS_LOCAL ${RESULTS} "${NEW_TARGET}")
            set(BOUND "${__BOUND__INTERNAL__}")
        endif()
    endforeach()
    if (RESULTS_LOCAL)
        list(REMOVE_DUPLICATES RESULTS_LOCAL)
    endif()
    set(__BOUND__INTERNAL__ "${BOUND}" PARENT_SCOPE)
    set(${OUTPUT} "${RESULTS_LOCAL}" PARENT_SCOPE)
endfunction()

#### Documentation links
# See Also:
#  - API: [API](../API.md) describes the `register_fprime_target` function
####
