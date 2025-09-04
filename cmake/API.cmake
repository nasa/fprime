####
# API.cmake:
#
# API of the fprime CMake system. These functions represent the external interface to all of the fprime CMake system.
# Users and developers should understand these functions in order to perform basic CMake setup while building as part
# of an fprime project.
#
# The standard patterns include:
# - Add a directory to the fprime system. Use this in place of add_subdirectory to get cleanly organized builds.
# - Register an fprime module/executable/ut to receive the benefits of autocoding.
# - Register an fprime build target/build stage to allow custom build steps. (Experimental)
#
####
include_guard()
include(utilities)
include(module)
include(config_assembler)
include(sub-build/sub-build)
include(fprime-util)
set(FPRIME_TARGET_LIST "" CACHE INTERNAL "FPRIME_TARGET_LIST: custom fprime targets" FORCE)
set(FPRIME_UT_TARGET_LIST "" CACHE INTERNAL "FPRIME_UT_TARGET_LIST: custom fprime targets" FORCE)
set(FPRIME_AUTOCODER_TARGET_LIST "" CACHE INTERNAL "FPRIME_AUTOCODER_TARGET_LIST: custom fprime targets" FORCE)

####
# Macro `skip_on_sub_build`:
#
# Skip this remaining code in the current function or file when executing in the context of a sub build. Sub builds
# execute utility and setup functions in fprime. However, certain CMake functions are not appropriate in this context
# and should be skipped.
####
macro(skip_on_sub_build)
    if (FPRIME_IS_SUB_BUILD)
        return()
    endif()
endmacro()

####
# Macro `restrict_platforms`:
#
# Restricts a CMakeLists.txt file to a given list of supported platforms, toolchains, and features. This prevents
# usage on platforms/toolchains  for which the module is incapable of being used and replaces the historical pattern of
# an if-tree detecting unsupported platforms in most circumstances.
#
# Valid inputs include names of platforms (e.g. Linux), names of specific toolchains (e.g. aarch64-linux), and platform
# supported feature sets (e.g. SOCKETS, which inspects the FPRIME_HAS_SOCKETS flag).
#
# Usage:
#    restrict_platforms(Linux Darwin) # Restricts to Linux and Darwin platforms
#        -or-
#    restrict_platforms(Posix) # Restricts to posix systems
#        -or-
#    restrict_platforms(SOCKETS) # Restricts to platforms where FPRIME_HAS_SOCKETS is TRUE
#
# Args:
#   ARGN: list of platforms that are supported
#####
macro(restrict_platforms)
    set(__CHECKER ${ARGN})

    # Determine if any of the restrict-tos maps to a fprime feature flag of the form FPRIME_HAS_XYZ as set in the
    # platform support file. If this feature is set and true, then the restriction block may pass.
    set(__HAS_SUPPORTED_FEATURE FALSE)
    foreach (__RESTRICTION IN LISTS __CHECKER)
        string(TOUPPER "${__RESTRICTION}" __RESTRICTION_UPPER)
        if (FPRIME_HAS_${__RESTRICTION_UPPER})
            set(__HAS_SUPPORTED_FEATURE TRUE)
            break()
        endif()
    endforeach()
    # Each of these empty if blocks are the valid-case, that is, the platform is supported.
    # However, the reason why this is necessary is that this is implemented as a macro and not a function.
    # Macros copy-paste the code into the calling context. Thus, all these valid cases want to avoid calling return.
    # The return call  in the else block returns from the calling context (i.e. a restricted CMakeList.txt will
    # return and not process the component setup). We do not want this return when the platform is allowed.

    if (FPRIME_TOOLCHAIN_NAME IN_LIST __CHECKER)
    elseif(FPRIME_PLATFORM IN_LIST __CHECKER)
    # New style FPRIME_HAS_<FEATURE>
    elseif(__HAS_SUPPORTED_FEATURE)
    # Old style posix FPRIME_USE_POSIX
    elseif("Posix" IN_LIST __CHECKER AND FPRIME_USE_POSIX)
    else()
        get_module_name("${CMAKE_CURRENT_LIST_DIR}")
        message(STATUS "Neither toolchain ${FPRIME_TOOLCHAIN_NAME} nor platform ${FPRIME_PLATFORM} supported for module ${MODULE_NAME}")
        append_list_property("${MODULE_NAME}" GLOBAL PROPERTY RESTRICTED_TARGETS)
        return()
    endif()
endmacro()

####
# Function `add_fprime_subdirectory`:
#
# Adds a subdirectory to the build system. This allows the system to find new available modules,
# executables, and unit tests. Every module, used or not, by the deployment/root CMAKE file should
# be added as a subdirectory somewhere in the tree. CMake's dependency system will prevent superfluous building, and
# `add_fprime_subdirectory` calls construct the super-graph from which the build graph is realized. Thus
# it is inconsequential to add a subdirectory that will not be used, but all code should be found within this
# super-graph to be available to the build.
#
# Every subdirectory added should declare a `CMakeLists.txt`. These in-turn may add their own sub-
# directories. This creates a directed acyclic graph of modules, one subgraph of which will be built
# for each executable/module/library defined in the system.  The subgraph should also be a DAG.
#
# This directory is computed based off the closest path in `FPRIME_BUILD_LOCATIONS`. It must be set to
# be used. Otherwise, an error will occur. `EXCLUDE_FROM_ALL` can also be supplied.
# See: https://cmake.org/cmake/help/latest/command/add_fprime_subdirectory.html
#
# **Note:** Replaces CMake `add_subdirectory` call in order to automate the [binary_dir] argument.
#           fprime subdirectories have specific binary roots to avoid collisions, and provide for
#           the standard fprime #include paths rooted at the root of the repo.
#
# **Arguments:**
#  - **FP_SOURCE_DIR:** directory to add (same as add_directory)
#  - **EXCLUDE_FROM_ALL:** (optional) exclude any targets from 'all'. See:
#                          https://cmake.org/cmake/help/latest/command/add_fprime_subdirectory.html
####
function(add_fprime_subdirectory FP_SOURCE_DIR)
    get_module_name("${FP_SOURCE_DIR}")
    set(FPRIME_CURRENT_MODULE "${MODULE_NAME}")

    # Unset all variables that carry special meaning as it is dangerous to pass them through
    foreach (VARIABLE IN ITEMS SOURCE_FILES MOD_DEPS UT_SOURCE_FILES UT_MOD_DEPS EXECUTABLE_NAME)
        set(${VARIABLE} PARENT_SCOPE)
    endforeach()
    get_filename_component(ABSOLUTE_SOURCE_PATH "${FP_SOURCE_DIR}" ABSOLUTE)
    file(RELATIVE_PATH NEW_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${ABSOLUTE_SOURCE_PATH}")

    # Check if the binary and source directory are in agreement. If they agree, then normally add
    # the directory, as no adjustments need be made.
    get_filename_component(CBD_NAME "${CMAKE_CURRENT_BINARY_DIR}" NAME)
    get_filename_component(CSD_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
    if ("${CBD_NAME}" STREQUAL "${CSD_NAME}")
        fprime_util_metadata_add_subdirectory("${FP_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/${NEW_BIN_DIR}")
        add_subdirectory(${ARGV}) # List of all args, not just extras
        return()
    endif()
    if (${ARGC} GREATER 2)
        message(FATAL_ERROR "Cannot use 'add_fprime_subdirectory' with [binary_dir] argument.")
    endif()
    # Make the path resolved: absolute, links resolved, etc.
    # This allows the relative path commands (below) to work correctly.
    resolve_path_variables(FP_SOURCE_DIR)
    get_nearest_build_root("${FP_SOURCE_DIR}")
    file(RELATIVE_PATH NEW_BIN_DIR "${FPRIME_CLOSEST_BUILD_ROOT}" "${FP_SOURCE_DIR}")
    # Add component subdirectories using normal add_subdirectory with overridden binary_dir
    fprime_util_metadata_add_subdirectory("${FP_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/${NEW_BIN_DIR}")
    add_subdirectory("${FP_SOURCE_DIR}" "${NEW_BIN_DIR}" ${ARGN})
endfunction(add_fprime_subdirectory)

####
# Function `fprime_attach_custom_targets`:
#
# Attaches custom fprime targets (cmake/targets) and their associated autocoding to the supplied build
# target. This is done automatically by the `register_fprime_*` family of functions and provides deferred
# target setup for use with `fprime_add_*_build_target` family functions.
#
# **BUILD_TARGET_NAME:** name of build target to attach targets and autocoding to
#
####
function(fprime_attach_custom_targets BUILD_TARGET_NAME)
    setup_module_targets("${BUILD_TARGET_NAME}")
endfunction()

####
# Function `register_fprime_library`:
#
# Registers a library using the fprime build system. This comes with dependency management and fprime
# autocoding capabilities. The first argument is the name of this module and will become the build target
# name. Sources, autocoder inputs, link dependencies, and headers are each passed in after the directives
# SOURCES, AUTOCODER_INPUTS, DEPENDS, and HEADERS, respectively.  Each directive may be used one time and
# dictates the contents of arguments until the next directive.
#
# **Example:**
#
# ```
# register_fprime_library(
#         MyFprimeModule
#     SOURCES
#         source1.cpp
#         source2.cpp
#     AUTOCODER_INPUTS
#         model.fpp
#     DEPENDS
#         -lm
#     HEADERS
#         module.h
# )
# ```
#
# > [!NOTE]
# > This delegates to CMake's `add_library` call. The library argument EXCLUDE_FROM_ALL is supported.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(register_fprime_library)
    fprime_add_library_build_target(${ARGN})
    clear_historical_variables()
    # Set up target/ targets for this module
    fprime_attach_custom_targets("${INTERNAL_MODULE_NAME}")
endfunction(register_fprime_library)

####
# Function `register_fprime_module`:
#
# See `register_fprime_library`. This provides the same capability as `register_fprime_library` using the
# backwards-compatible name.
#
# > [!NOTE]
# > Variables SOURCE_FILES, MOD_DEPS, etc. are still supported but are no longer recommended.  Users are
# > encouraged to update at their convenience.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(register_fprime_module)
    register_fprime_library(${ARGN})
endfunction(register_fprime_module)

####
# Function `fprime_add_library_build_target`:
#
# Registers a library using the fprime build system without setting up autocoding or target
# support. See `register_fprime_library`.
#
# > [!NOTE]
# > Users may set up custom target and autocoder support by calling `fprime_attach_custom_targets`.
#
# This function sets "INTERNAL_MODULE_NAME" in PARENT_SCOPE to pass-back module name for target
# registration.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(fprime_add_library_build_target)
    fprime__internal_add_build_target("Library" "INTERFACE;OBJECT" ${ARGN})
    clear_historical_variables()
    set(INTERNAL_MODULE_NAME "${INTERNAL_MODULE_NAME}" PARENT_SCOPE)
endfunction()

####
# Function `register_fprime_executable`:
#
# Registers an executable using the fprime build system. This comes with dependency management and fprime
# autocoding capabilities. The call format is identical to `register_fprime_library`.
#
# **Example:**
#
# ```
# register_fprime_executable(
#         MyFprimeExecutable
#     SOURCES
#         source1.cpp
#         source2.cpp
#     AUTOCODER_INPUTS
#         model.fpp
#     DEPENDS
#         -lm
#     HEADERS
#         module.h
# )
# ```
#
# > [!NOTE]
# > This delegates to CMake's `add_executable` call. The argument EXCLUDE_FROM_ALL is supported.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(register_fprime_executable)
    if (DEFINED EXECUTABLE_NAME)
        fprime_cmake_fatal_error("EXECUTABLE_NAME variable no longer supported")
    endif()
    fprime_add_executable_build_target(${ARGN})

    # Set up target/ targets for this module
    fprime_attach_custom_targets("${INTERNAL_MODULE_NAME}")
endfunction(register_fprime_executable)

####
# Function `fprime_add_executable_build_target`:
#
# Registers a executable using the fprime build system without setting up autocoding or target
# support. See `register_fprime_executable`.
#
# > [!NOTE]
# > Users may set up custom target and autocoder support by calling `fprime_attach_custom_targets`.
#
# This function sets "INTERNAL_MODULE_NAME" in PARENT_SCOPE to pass-back module name for target
# registration.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(fprime_add_executable_build_target)
    fprime__internal_add_build_target("Executable" "CHOOSES_IMPLEMENTATIONS" ${ARGN})
    clear_historical_variables()
    set(INTERNAL_MODULE_NAME "${INTERNAL_MODULE_NAME}" PARENT_SCOPE)
endfunction()

####
# Function `register_fprime_deployment`:
#
# Registers a deployment using the fprime build system. This comes with dependency management and fprime
# autocoding capabilities. The call format is identical to `register_fprime_library`. Deployments come
# with custom target and autocoding support that allows them to run "targets" across their dependency
# trees (i.e. run all unit tests for components used in this deployment).
#
# **Example:**
#
# ```
# register_fprime_deployment(
#         MyFprimeDeployment
#     SOURCES
#         source1.cpp
#         source2.cpp
#     AUTOCODER_INPUTS
#         model.fpp
#     DEPENDS
#         MyFprimeDeployment_Top
#     HEADERS
#         module.h
# )
# ```
#
# > [!NOTE]
# > This delegates to CMake's `add_executable` call. The argument EXCLUDE_FROM_ALL is supported.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(register_fprime_deployment)
    # Fallback to PROJECT_NAME when it is not set
    if (NOT DEFINED FPRIME_CURRENT_MODULE)
        set(FPRIME_CURRENT_MODULE "${PROJECT_NAME}")
    endif()
    fprime_add_deployment_build_target(${ARGN})

    # Set up target/ targets for this module
    fprime_attach_custom_targets("${INTERNAL_MODULE_NAME}")
endfunction(register_fprime_deployment)

####
# Function `fprime_add_deployment_build_target`:
#
# Registers a deployment using the fprime build system without setting up autocoding or target
# support. See `register_fprime_deployment`.
#
# > [!NOTE]
# > Users may set up custom target and autocoder support by calling `fprime_attach_custom_targets`.
#
# This function sets "INTERNAL_MODULE_NAME" in PARENT_SCOPE to pass-back module name for target
# registration.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(fprime_add_deployment_build_target)
    fprime__internal_add_build_target("Deployment" "CHOOSES_IMPLEMENTATIONS" ${ARGN})
    clear_historical_variables()
    set(INTERNAL_MODULE_NAME "${INTERNAL_MODULE_NAME}" PARENT_SCOPE)
endfunction()

####
# Function `register_fprime_config`:
#
# Registers a configuration build target using the fprime build system. This comes with dependency management and
# fprime autocoding capabilities. The call format is identical to `register_fprime_library` and additionally supports
# the CONFIGURATION_OVERRIDES directive. This allows users to override the configuration files supplied by previous
# configuration modules supplied by the build (e.g. fprime default configuration and library configuration). DEPENDS
# and EXCLUDE_FROM_ALL are not supported.
#
# All configuration module sources (SOURCES, HEADERS, and AUTOCODER_INPUTS) are copied into the build cache.
# Overrides are copied into the original module's build that the file overrides as this preserves the original build
# module set up. Overrides only work in order of detection within the CMakeList.txt tree:
#
#    platform -> fprime config -> library -> project.
#
#
# > [!WARNING]
# > Specifying headers in this command is crucial to providing as configuration.
#
# > [!NOTE]
# > Configuration is built as a series of STATIC libraries in order to allow for interdependencies between config and
# > Fw_Types regardless of the Fw_Types library type.
#
# Example:
# ```
# register_fprime_config(
#         MyFprimeConfig
#     SOURCES
#         config.cpp
#     AUTOCODER_INPUTS
#         config.fpp
#     HEADERS
#         config.hpp
#     CONFIGURATION_OVERRIDES
#         FpConfig.fpp
#         FpConfig.hpp
# ```
####
function(register_fprime_config)
    fprime_add_config_build_target(${ARGN})
    # Clear the historical variables and set up autocode
    clear_historical_variables()
    fprime_attach_custom_targets("${INTERNAL_MODULE_NAME}")
endfunction()

####
# Function `fprime_add_config_build_target`:
#
# Registers config using the fprime build system without setting up autocoding or target
# support. See `register_fprime_config`.
#
# > [!NOTE]
# > Users may set up custom target and autocoder support by calling `fprime_attach_custom_targets`.
#
# This function sets "INTERNAL_MODULE_NAME" in PARENT_SCOPE to pass-back module name for target
# registration.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(fprime_add_config_build_target)
    set(ARGN_PASS ${ARGN})
    # Ensure library is STATIC when supplying SOURCE or AUTOCODER_INPUTS
    if (SOURCE IN_LIST ARGN_PASS OR AUTOCODER_INPUTS IN_LIST ARGN_PASS)
        if (NOT "STATIC" IN_LIST ARGN_PASS AND NOT INTERFACE IN_LIST ARGN_PASS)
            list(APPEND ARGN_PASS STATIC)
        endif()
    endif()
    #### Split module processing ####
    #
    # Configuration works by copying sources into the build cache. These new copied sources are
    # substituted for the original sources. Thus the module processing must happen before the
    # configuration processing, which is before the build target processing.
    #
    # This implies:
    # 1. The helper cannot be used as it combines module and build target processing
    # 2. Configuration processing must be called in-between
    ####
    fprime__process_module_setup("Library"
        "CONFIGURATION_OVERRIDES;STATIC;INTERFACE;CHOOSES_IMPLEMENTATIONS;BASE_CONFIG" ${ARGN_PASS})
    fprime__internal_process_configuration_sources(
        "${INTERNAL_MODULE_NAME}"
        "${INTERNAL_SOURCES}"
        "${INTERNAL_AUTOCODER_INPUTS}"
        "${INTERNAL_HEADERS}"
        "${INTERNAL_CONFIGURATION_OVERRIDES}"
        "${INTERNAL_DEPENDS}"
    )
    fprime__internal_add_build_target_helper("${INTERNAL_MODULE_NAME}" "Library" "${INTERNAL_SOURCES}"
                                             "${INTERNAL_AUTOCODER_INPUTS}" "${INTERNAL_HEADERS}" "${INTERNAL_DEPENDS}"
                                             "${INTERNAL_REQUIRES_IMPLEMENTATIONS}"
                                             "${INTERNAL_CHOOSES_IMPLEMENTATIONS}" "${INTERNAL_CMAKE_ADD_OPTIONS}")

    # The new module should include the root configuration directory
    fprime_target_include_directories("${INTERNAL_MODULE_NAME}" PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/..")
    # The configuration target should depend on the new module
    if (INTERNAL_BASE_CONFIG)
        target_link_libraries("${FPRIME__INTERNAL_CONFIG_TARGET_NAME}" INTERFACE "${INTERNAL_MODULE_NAME}")
    endif()
    # Set up the new module to be marked as FPRIME_CONFIGURATION
    append_list_property("${INTERNAL_MODULE_NAME}" GLOBAL PROPERTY "FPRIME_CONFIG_MODULES")
    set_property(TARGET "${INTERNAL_MODULE_NAME}" PROPERTY FPRIME_CONFIGURATION TRUE)
    # Targets likely do not exist yet, so just aggregate the complete list of chosen implementations
    # for processing later
    append_list_property("${INTERNAL_CHOOSES_IMPLEMENTATIONS}" TARGET "${FPRIME__INTERNAL_CONFIG_TARGET_NAME}" PROPERTY FPRIME_CHOSEN_IMPLEMENTATIONS)

    # Static libraries must be position independent when building shared libraries
    get_target_property(CONFIG_LIBRARY_TYPE "${INTERNAL_MODULE_NAME}" TYPE)
    if (BUILD_SHARED_LIBS AND CONFIG_LIBRARY_TYPE STREQUAL "STATIC_LIBRARY")
        target_compile_options(${INTERNAL_MODULE_NAME} PRIVATE -fPIC)
    endif()
    # Set INTERNAL_MODULE_NAME for caller
    set(INTERNAL_MODULE_NAME "${INTERNAL_MODULE_NAME}" PARENT_SCOPE)
endfunction()

####
# Function `register_fprime_ut`:
#
#
# Registers a unit test using the fprime build system. This comes with dependency management and fprime
# autocoding capabilities. The call format is identical to `register_fprime_library`. Unit tests come
# with custom target and autocoding support.
#
# This function only creates a target when unit test support is enabled on the build.
#
# **Example:**
#
# ```
# register_fprime_ut(
#         MyUnitTest
#     SOURCES
#         source1.cpp
#         source2.cpp
#     AUTOCODER_INPUTS
#         model.fpp
#     DEPENDS
#         MyFprimeModule
#     HEADERS
#         module.h
# )
# ```
#
# > [!NOTE]
# > This delegates to CMake's `add_executable` call. The argument EXCLUDE_FROM_ALL is supported.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(register_fprime_ut)
    # Bail out if not doing a unit test build
    # TODO: should this add a fake target?
    if (NOT BUILD_TESTING OR __FPRIME_NO_UT_GEN__)
        return()
    endif()
    fprime_add_unit_test_build_target(${ARGN})
    # Set up target/ targets for this module
    fprime_attach_custom_targets("${INTERNAL_MODULE_NAME}")
endfunction(register_fprime_ut)

####
# Function `fprime_add_unit_test_build_target`:
#
# Registers a unit test using the fprime build system without setting up autocoding or target
# support. See `register_fprime_ut`.
#
# > [!NOTE]
# > Users may set up custom target and autocoder support by calling `fprime_attach_custom_targets`.
#
# This function sets "INTERNAL_MODULE_NAME" in PARENT_SCOPE to pass-back module name for target
# registration.
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(fprime_add_unit_test_build_target)
    fprime__internal_add_build_target("Unit Test" "INCLUDE_GTEST;UT_AUTO_HELPERS;CHOOSES_IMPLEMENTATIONS;TESTED_MODULE" ${ARGN})
    clear_historical_variables()
    set(INTERNAL_MODULE_NAME "${INTERNAL_MODULE_NAME}" PARENT_SCOPE)
endfunction()


####
# Macro `register_fprime_target`:
#
# This function allows users to register custom build targets into the build system.  These targets are defined in a
# CMake file and consist of three functions that operate on different parts of the build: global, per-module, and
# per-deployment. See: [Targets](./target/target.md).
#
# This function takes in either a file path to a CMake file defining targets, or an short include path that accomplishes
# the same thing. Note: make sure the directory is on the CMake include path to use the second form. The supplied file
# should define three functions: `add_global_target`, `add_module_target`, and `add_deployment_target`.
#
# **TARGET_FILE_PATH:** include path or file path file defining above functions
###
macro(register_fprime_target TARGET_FILE_PATH)
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[target] Registering custom target: ${TARGET_FILE_PATH}")
    endif()
    register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_TARGET_LIST OFF)
endmacro(register_fprime_target)

####
# Macro `register_fprime_ut_target`:
#
# Identical to the above `register_fprime_target` function except that these targets are only created when the system
# is building unit tests. e.g. BUILD_TESTING=ON.
#
# **TARGET_FILE_PATH:** include path or file path files
###
macro(register_fprime_ut_target TARGET_FILE_PATH)
    # UT targets only allowed when testing
    if (BUILD_TESTING)
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[target] Registering custom target: ${TARGET_FILE_PATH}")
        endif()
        register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_UT_TARGET_LIST OFF)
    endif()
endmacro(register_fprime_ut_target)

####
# Macro `register_fprime_list_helper`:
#
# Helper function to do the actual registration. Also used to side-load prescan to bypass the not-on-prescan check. Takes in a boolean argument TO_PREPEND to determine if the target should be prepended to the list.
####
macro(register_fprime_list_helper TARGET_FILE_PATH TARGET_LIST TO_PREPEND)
    if (NOT DEFINED FPRIME_SUB_BUILD_TARGETS OR "${TARGET_FILE_PATH}" IN_LIST FPRIME_SUB_BUILD_TARGETS)
        include("${TARGET_FILE_PATH}")
        # Prevent out-of-order setups
        get_property(MODULE_DETECTION_STARTED GLOBAL PROPERTY MODULE_DETECTION SET)
        if (MODULE_DETECTION_STARTED)
            message(FATAL_ERROR "Cannot register fprime target after including subdirectories or FPrime-Code.cmake'")
        endif()
        get_property(TARGETS GLOBAL PROPERTY "${TARGET_LIST}")
        if (NOT TARGET_FILE_PATH IN_LIST TARGETS)
            if (${TO_PREPEND})
                get_property(TMP_LIST GLOBAL PROPERTY "${TARGET_LIST}")
                set(TMP_LIST "${TARGET_FILE_PATH}" ${TMP_LIST})
                set_property(GLOBAL PROPERTY "${TARGET_LIST}" ${TMP_LIST})
            else()
                set_property(GLOBAL APPEND PROPERTY "${TARGET_LIST}" "${TARGET_FILE_PATH}")
            endif()
        endif()
    endif()
endmacro(register_fprime_list_helper)


####
# Macro `register_fprime_build_autocoder`:
#
# This function allows users to register custom autocoders into the build system. These autocoders will execute during
# the build process. An autocoder is defined in a CMake file and must do three things:
# 1. Call one of `autocoder_setup_for_individual_sources()` or `autocoder_setup_for_multiple_sources()` from file scope
# 2. Implement `<autocoder name>_is_supported(AC_POSSIBLE_INPUT_FILE)` returning true the autocoder processes given source
# 3. Implement `<autocoder name>_setup_autocode AC_INPUT_FILE)` to run the autocoder on files filter by item 2.
#
# This function takes in either a file path to a CMake file defining an autocoder target, or an short include path that accomplishes
# the same thing. Note: make sure the directory is on the CMake include path to use the second form.
#
# **TARGET_FILE_PATH:** include path or file path file defining above functions
####
macro(register_fprime_build_autocoder TARGET_FILE_PATH TO_PREPEND)
    # Normal registered targets don't run in pre-builds
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[autocoder] Registering custom build target autocoder: ${TARGET_FILE_PATH} prepend: ${TO_PREPEND}")
    endif()
    register_fprime_list_helper("${TARGET_FILE_PATH}" FPRIME_AUTOCODER_TARGET_LIST ${TO_PREPEND})
endmacro(register_fprime_build_autocoder)

####
# Function `create_implementation_interface`:
#
# Helper function to create implementation interface library once and only once to ensure it exists.
#
# **IMPLEMENTATION**: implementation library name (resolved)
####
function (create_implementation_interface IMPLEMENTATION)
    if (TARGET "${IMPLEMENTATION}")
        return()
    endif()
    add_library("${IMPLEMENTATION}" INTERFACE)
endfunction()

####
# Function `register_fprime_implementation`:
#
# Designates that the given implementor implements the required implementation and registers it as a library. This
# library will always be of type OBJECT to ensure that it will override at link time as expected. The call format is
# identical to `register_fprime_library`, but requires the IMPLEMENTS <implementation interface> directive to indicate
# which implementation is being implemented.
#
# > [!WARNING]
# > The result of this call will always be an OBJECT library.
#
# **Example:**
#
# ```
# register_fprime_implementation(
#         MyImplementation
#     IMPLEMENTS
#         SomeImplementationInterface
#     SOURCES
#         source1.cpp
#         source2.cpp
#     AUTOCODER_INPUTS
#         model.fpp
#     HEADERS
#         module.h
# )
# ```
#
# **MODULE_NAME**: (optional) module name. Default: ${FPRIME_CURRENT_MODULE}
# **ARGN**: sources, autocoder inputs, etc preceded by a directive (i.e. SOURCES or DEPENDS)
#
####
function(register_fprime_implementation)
    # Update ARGN to include OBJECT and EX
    set(ARGN_PASS "${ARGN}")
    if (NOT "OBJECT" IN_LIST ARGN_PASS AND NOT INTERFACE IN_LIST ARGN_PASS)
        list(APPEND ARGN_PASS OBJECT)
    endif()
    fprime__internal_add_build_target("Library" "IMPLEMENTS;OBJECT;INTERFACE" ${ARGN_PASS})

    #### Special implementation handling ####

    # Validate the number of implementations passed to "IMPLEMENTS"
    list(LENGTH INTERNAL_IMPLEMENTS INTERNAL_IMPLEMENTS_LENGTH)
    if (NOT INTERNAL_IMPLEMENTS_LENGTH EQUAL 1 OR "${INTERNAL_IMPLEMENTS}" STREQUAL "TRUE")
        fprime_cmake_fatal_error("Must supply exactly 1 argument to the IMPLEMENTS directive")
    endif()
    # Check implementation properties still in-sync before setting the target-driven equivalents
    get_property(OLD_IMPLEMENTS GLOBAL PROPERTY FPRIME_${INTERNAL_MODULE_NAME}_IMPLEMENTS)
    fprime_cmake_ASSERT(
        "${INTERNAL_MODULE_NAME} implementation changed from: ${OLD_IMPLEMENTS} to ${INTERNAL_IMPLEMENTS}"
        NOT OLD_IMPLEMENTS OR OLD_IMPLEMENTS STREQUAL INTERNAL_IMPLEMENTS
    )
    set_target_properties("${INTERNAL_MODULE_NAME}" PROPERTIES FPRIME_IMPLEMENTS "${INTERNAL_IMPLEMENTS}")
    append_list_property("${INTERNAL_MODULE_NAME}" GLOBAL PROPERTY "FPRIME_${INTERNAL_IMPLEMENTS}_IMPLEMENTORS")

    fprime_attach_custom_targets("${INTERNAL_MODULE_NAME}")
endfunction()

####
# Adds a named os implementation.
#
# Assumptions:
#   1. NAMES is a list of 1 or more named files separated by ;
#   2. There exists a file named Default${FIRST_ITEM}, where FIRST_ITEM is the first element in NAME, in the same
#       directory where this cmake function was called
#   3. For each item e listed in NAMES, there exists a file called ${e}.hpp and ${e}.cpp in the same directory
#       where this cmake function was called
#
# NAMES: list of named files to add to this module.  The first will be treated as the name of the module.
#        i.e. File;Directory;FileSystem will contain the file, directory, and filesystem files in a module called File.
# SUFFIX: suffix to implementation (e.g. Posix)
# ARGN: extra MOD_DEPS to add (e.g. Fw_Time)
####
function(register_os_implementation NAMES SUFFIX)
    add_fprime_supplied_os_module("${NAMES}" "${SUFFIX}" "${ARGN}")
endfunction()

#### Documentation links
# Next Topics:
#  - Setting Options: [Options](options.md) are used to vary a CMake build.
#  - Adding Modules: [Modules](module.md) register fprime Ports, Components, etc.
#  - Creating Toolchains: [Toolchains](../../../user-manual/build-system/cmake-toolchains.md) setup standard CMake Cross-Compiling.
#  - Adding Platforms: [Platforms](../../../user-manual/build-system/cmake-platforms.md) help fprime set Cross-Compiling specific items.
#  - Adding Targets: [Targets](./target/target.md) for help defining custom build targets
#  - Implementation Packages Design: [Implementation Packages](../../../user-manual/build-system/package-implementations.md)
####
