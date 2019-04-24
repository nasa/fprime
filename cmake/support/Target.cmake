####
# Target.cmake:
#
# Functions supporting the F prime target additions. These targets allow building agains modules
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
# Adds a module-by-module target for this given target. Any subtargets for each module and thier
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
    include("${TARGET_FILE_PATH}")
    if (NOT COMMAND add_module_target)
        message(FATAL_ERROR "${TARGET_FILE_PATH} must define 'add_module_target'")
    endif()
    get_target_name("${TARGET_FILE_PATH}")
    # User did not define a global target function, add a blank one
    if (NOT COMMAND add_global_target)
        message(STATUS "Adding blank target: ${TARGET_NAME}")
    	add_custom_target(${TARGET_NAME})
    # Otherwise use user's specification
    else()
        add_global_target(${TARGET_NAME})
    endif()
endfunction(setup_global_target)

####
# Function `setup_module_target`:
#
# Sets up an individual module target. This is called in a loop to add all targets. **Note:** this
# function must be singular, as scoping rules allow inclusion without collision.
#
# - **MODULE_NAME:** name of module
# - **TARGET_FILE_PATH:** path to target file
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
####
function(setup_module_target MODULE_NAME TARGET_FILE_PATH AC_INPUTS SOURCE_FILES AC_OUTPUTS)
	# Include the file and look for definitions
    include("${TARGET_FILE_PATH}")
    get_target_name("${TARGET_FILE_PATH}")
    add_module_target(${MODULE_NAME} "${MODULE_NAME}_${TARGET_NAME}" "${AC_INPUTS}" "${SOURCE_FILES}" "${AC_OUTPUTS}")
endfunction(setup_module_target)

####
# Function `setup_all_module_targets`:
#
# Takes all registerd targets and sets up the module specific target from them. The list of targets
# is read from the CACHE variable FPRIME_TARGET_LIST.
#
# - **MODULE_NAME:** name of the module
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
####
function(setup_all_module_targets MODULE_NAME AC_INPUTS SOURCE_FILES AC_OUTPUTS)
    foreach(ITEM ${FPRIME_TARGET_LIST})
        setup_module_target(${MODULE_NAME} ${ITEM} "${AC_INPUTS}" "${SOURCE_FILES}" "${AC_OUTPUTS}")
        get_target_name("${ITEM}")
        if (TARGET "${MODULE_NAME}_${TARGET_NAME}")
            add_dependencies("${TARGET_NAME}" "${MODULE_NAME}_${TARGET_NAME}")
        endif()
    endforeach(ITEM ${FPRIME_TARGET_LIST})
endfunction(setup_all_module_targets)
#### Documentation links
# See Also:
#  - API: [API](../API.md) describes the `register_fprime_target` function
####