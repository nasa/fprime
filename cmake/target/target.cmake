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
# Function `setup_global_target`:
#
# Sets up the global target with default target or custom target
# - **TARGET_FILE_NAME:** name of target file
####
function(setup_global_target TARGET_FILE_PATH)
    # Include the file and look for definitions
    include(target/default)
    include("${TARGET_FILE_PATH}")
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[target] Setting up '${TARGET_FILE_PATH}' global target")
    endif()
    get_target_name("${TARGET_FILE_PATH}")
    add_global_target(${TARGET_NAME})
endfunction(setup_global_target)

####
# Function `setup_deployment_target`:
#
# Sets up an individual deployment target.
####
function(setup_deployment_target MODULE_NAME TARGET_FILE_PATH SOURCE_FILES DEPENDENCIES FULL_DEPENDENCIES)
    # Include the file and look for definitions
    include(target/default)
    include("${TARGET_FILE_PATH}")
    get_target_name("${TARGET_FILE_PATH}")
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[target] Setting up '${TARGET_NAME}' on ${MODULE_NAME}")
    endif()
    add_deployment_target(${MODULE_NAME} "${TARGET_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}" "${FULL_DEPENDENCIES}")
endfunction(setup_deployment_target)

####
# Function `setup_all_deployment_targets`:
#
# Takes all registered targets and sets up the module specific target from them. The list of targets
# is read from the CACHE variable FPRIME_TARGET_LIST.
#
# - **TARGET_LIST:** list of targets to setup
# - **MODULE_NAME:** name of the module
# - **SOURCE_FILES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(setup_all_deployment_targets TARGET_LIST MODULE_NAME SOURCE_FILES DEPENDENCIES)
    resolve_dependencies(RESOLVED ${DEPENDENCIES})
    recurse_targets("${MODULE_NAME}" RESULTS "" ${RESOLVED})
    foreach(ITEM ${${TARGET_LIST}})
        setup_deployment_target(${MODULE_NAME} ${ITEM} "${SOURCE_FILES}" "${DEPENDENCIES}" "${RESULTS}")
    endforeach(ITEM ${${TARGET_LIST}})
endfunction(setup_all_deployment_targets)


####
# Function `setup_module_target`:
#
# Sets up an individual module target. This is called in a loop to add all targets. **Note:** this
# function must be singular, as scoping rules allow inclusion without collision.
#
# - **MODULE_NAME:** name of module
# - **TARGET_FILE_PATH:** path to target file
# - **SOURCE_FILES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(setup_module_target MODULE_NAME TARGET_FILE_PATH SOURCE_FILES DEPENDENCIES)
    # Include the file and look for definitions
    include(target/default)
    include("${TARGET_FILE_PATH}")
    get_target_name("${TARGET_FILE_PATH}")
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[target] Setting up '${TARGET_NAME}' on ${MODULE_NAME}")
    endif()
    add_module_target(${MODULE_NAME} "${TARGET_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
endfunction(setup_module_target)

####
# Function `setup_all_module_targets`:
#
# Takes all registered targets and sets up the module specific target from them. The list of targets
# is read from the CACHE variable FPRIME_TARGET_LIST.
#
# - **TARGET_LIST:** list of targets to setup
# - **MODULE_NAME:** name of the module
# - **SOURCE_FILES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(setup_all_module_targets TARGET_LIST MODULE_NAME SOURCE_FILES DEPENDENCIES)
    foreach(ITEM ${${TARGET_LIST}})
        setup_module_target(${MODULE_NAME} ${ITEM} "${SOURCE_FILES}" "${DEPENDENCIES}")
    endforeach(ITEM ${${TARGET_LIST}})
    set_property(GLOBAL APPEND PROPERTY FPRIME_MODULES ${MODULE_NAME})
endfunction(setup_all_module_targets)

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
        endif()
    endforeach()
    if (RESULTS_LOCAL)
        list(REMOVE_DUPLICATES RESULTS_LOCAL)
    endif()
    set(${OUTPUT} "${RESULTS_LOCAL}" PARENT_SCOPE)
endfunction()

#### Documentation links
# See Also:
#  - API: [API](../API.md) describes the `register_fprime_target` function
####
