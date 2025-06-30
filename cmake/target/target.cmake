####
# Target.cmake:
#
# Functions supporting the F prime target additions. These targets allow building against modules
# and top-level targets. This allows for multi-part builds like `sloc` or `dict` where some part
# applies to the module and is rolled up into some global command. 
#
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
# Function `check_unknown_links`:
#
# Checks all ARGN supplied arguments to determine if they **should have** existed for deployment recursion to work
# properly. If an argument is a file (i.e. a library or other file), a linker flag (-*), a generator expression
# ($*) then these are passed. Anything else results in an error.
#
# - **DEPLOYMENT_NAME:** name of deployment being recursed for cleaner error messages
# - **ARGN:** list of unknown targets to check
####
function(check_unknown_links DEPLOYMENT_NAME)
    # Check all links that they exist or are valid
    foreach(LINK IN LISTS ARGN)
        # When a link is not a file, not a link flag, and not a generator expression then the target must already exist
        # as a target in the CMake system to be used as part of recursive dependency lists.
        if (EXISTS "${LINK}" AND NOT IS_DIRECTORY "${LINK}")
            # File detected, skip dependency
        elseif("${LINK}" MATCHES "^[-$].*")
            # Link library of some form detected
        else()
            # Internal dependency name, must exist thus a failure
            fprime_cmake_fatal_error(
                "F Prime/CMake target '${LINK}' not available to deployment '${DEPLOYMENT_NAME}'. '${LINK}' must:\n"
                "    1. Must be defined somewhere in the F Prime project\n"
                "    2. Must be defined before '${DEPLOYMENT_NAME}' deployment (register_fprime_deployment)\n"
                "'${LINK}' is undefined, or included via `add_fprime_subdirectory` after `register_fprime_deployment`."
            )
        endif()
    endforeach()
endfunction()


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
    get_target_property(MODULE_TYPE "${MODULE}" FPRIME_TYPE)

    if (NOT MODULE_TYPE STREQUAL "Deployment")
        cmake_language(CALL "${TARGET_NAME}_add_module_target" "${MODULE}" "${TARGET_NAME}" "${SOURCES}" "${DEPENDENCIES}")
    else()
        get_target_property(TRANSITIVE_DEPENDENCIES "${MODULE}" TRANSITIVE_DEPENDENCIES)
        # Recalculate recursive dependencies
        if (NOT TRANSITIVE_DEPENDENCIES)
            set(RECURSED_PROPERTY_NAMES FPRIME_DEPENDENCIES)
            recurse_target_properties("${MODULE}" "${RECURSED_PROPERTY_NAMES}" KNOWN_TRANSITIVE_LINKS EXTERNAL_LINKS UNKNOWN_LINKS)
            
            # Report all detected recursive dependencies
            if (CMAKE_DEBUG_OUTPUT)
                foreach(LIST_PRINT IN ITEMS EXTERNAL_LINKS KNOWN_TRANSITIVE_LINKS UNKNOWN_LINKS)
                    message(STATUS "[target] '${MODULE}' Recursive Links: ${LIST_PRINT}")
                    foreach(ITEM_PRINT IN LISTS ${LIST_PRINT})
                        message(STATUS "[target]    ${ITEM_PRINT}")
                    endforeach()
                endforeach()
            endif()
            check_unknown_links("${MODULE}" ${UNKNOWN_LINKS})
            set_target_properties("${MODULE}" PROPERTIES TRANSITIVE_DEPENDENCIES "${KNOWN_TRANSITIVE_LINKS}")
            set(TRANSITIVE_DEPENDENCIES "${KNOWN_TRANSITIVE_LINKS}")
        endif()
        cmake_language(CALL "${TARGET_NAME}_add_deployment_target" "${MODULE}" "${TARGET_NAME}" "${SOURCES}" "${DEPENDENCIES}" "${TRANSITIVE_DEPENDENCIES}")
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
function(setup_module_targets BUILD_TARGET)
    # Grab the list of targets
    set(LIST_NAME FPRIME_TARGET_LIST)

    # Read target properties
    foreach(PROPERTY IN ITEMS FPRIME_TYPE SOURCES LINK_LIBRARIES INTERFACE_LINK_LIBRARIES)
        get_target_property("MODULE_${PROPERTY}" "${BUILD_TARGET}" "${PROPERTY}")
        if (NOT MODULE_${PROPERTY})
            set("MODULE_${PROPERTY}")
        endif()
    endforeach()


    # Get both normal and ut target lists
    get_property(TARGETS GLOBAL PROPERTY FPRIME_TARGET_LIST)
    get_property(UT_TARGETS GLOBAL PROPERTY FPRIME_UT_TARGET_LIST)
    # UT targets are the only targets run on unit tests, and are included in deployments
    if (MODULE_FPRIME_TYPE STREQUAL "Deployment")
        list(APPEND TARGETS ${UT_TARGETS})
    elseif (MODULE_FPRIME_TYPE STREQUAL "Unit Test")
        set(TARGETS "${UT_TARGETS}")
    endif()

    # Now run through each of the determined targets
    set(DEPENDENCIES ${MODULE_LINK_LIBRARIES} ${MODULE_INTERFACE_LINK_LIBRARIES})
    list(REMOVE_DUPLICATES DEPENDENCIES)
    foreach(FPRIME_TARGET IN LISTS TARGETS)
        setup_single_target("${FPRIME_TARGET}" "${BUILD_TARGET}" "${MODULE_SOURCES}" "${DEPENDENCIES}")
    endforeach()
endfunction(setup_module_targets)
#### Documentation links
# See Also:
#  - API: [API](../API.md) describes the `register_fprime_target` function
####
