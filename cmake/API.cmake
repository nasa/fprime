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
    if (DEFINED FPRIME_SUB_BUILD_TARGETS)
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

    # Check if the binary and source directory are in agreement. If they agree, then normally add
    # the directory, as no adjustments need be made.
    get_filename_component(CBD_NAME "${CMAKE_CURRENT_BINARY_DIR}" NAME)
    get_filename_component(CSD_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
    if ("${CBD_NAME}" STREQUAL "${CSD_NAME}")
        add_subdirectory(${ARGV}) # List of all args, not just extras
        return()
    endif()
    if (${ARGC} GREATER 2)
        message(FATAL_ERROR "Cannot use 'add_fprime_subdirectory' with [binary_dir] argument.")
    endif()
    get_nearest_build_root("${FP_SOURCE_DIR}")
    file(RELATIVE_PATH NEW_BIN_DIR "${FPRIME_CLOSEST_BUILD_ROOT}" "${FP_SOURCE_DIR}")
    # Add component subdirectories using normal add_subdirectory with overridden binary_dir
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
    fprime__internal_add_build_target("Library" "" ${ARGN})
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
    fprime__internal_add_build_target("Executable" "" ${ARGN})
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
    fprime__internal_add_build_target("Deployment" "" ${ARGN})
    clear_historical_variables()
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
    fprime__internal_add_build_target("Unit Test" "INCLUDE_GTEST" ${ARGN})
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
# Function `require_fprime_implementation`:
#
# Designates that the current module requires a separate implementation in order for it to function properly. As an
# example, Os requires an implementation of `Os_Task`. These implementations must be set via
# `choose_fprime_implementation` in the platform and may be overridden in in the executable/deployment.
#
# **IMPLEMENTATION:** implementation module name that must be covered
# **REQUESTER:** (optional) the requester of the implementation. Default: ${FPRIME_CURRENT_MODULE}
####
function(require_fprime_implementation IMPLEMENTATION)
    if (ARGC EQUAL 2)
        set(REQUESTER "${ARGV1}")
    elseif (FPRIME_CURRENT_MODULE)
        set(REQUESTER "${FPRIME_CURRENT_MODULE}")
    else ()
        message(FATAL_ERROR "Cannot determine current module, please supply as second argument")
    endif()
    resolve_dependencies(IMPLEMENTATION "${IMPLEMENTATION}")
    resolve_dependencies(REQUESTER "${REQUESTER}")
    create_implementation_interface("${IMPLEMENTATION}")
    append_list_property("${IMPLEMENTATION}" GLOBAL PROPERTY "REQUIRED_IMPLEMENTATIONS")
    add_dependencies("${REQUESTER}" "${IMPLEMENTATION}")
endfunction()

####
# Function `register_fprime_implementation`:
#
# Designates that the given implementor implements the required implementation. As an example Os_Task_Posix implements
# Os_Task. These implementations must be set via
## `choose_fprime_implementation` in the platform and may be overridden in in the executable/deployment.
#
# **IMPLEMENTATION:** implementation module name that is implemented by IMPLEMENTOR
# **IMPLEMENTOR:** implementor of IMPLEMENTATION
# **ARGN:** (optional) list of source files required to build the implementor
####
function(register_fprime_implementation IMPLEMENTATION IMPLEMENTOR)
    resolve_dependencies(IMPLEMENTATION "${IMPLEMENTATION}")
    resolve_dependencies(IMPLEMENTOR "${IMPLEMENTOR}")
    create_implementation_interface("${IMPLEMENTATION}")
    append_list_property("${IMPLEMENTOR}" GLOBAL PROPERTY "${IMPLEMENTATION}_IMPLEMENTORS")
    append_list_property("${ARGN}" TARGET "${IMPLEMENTOR}" PROPERTY "REQUIRED_SOURCE_FILES")
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

####
# Function `choose_fprime_implementation`:
#
# Designates that the given implementor is the selected implementor for the needed implementation. Platforms must call
# this function once for each defined IMPLEMENTATION. An executable/deployment/unit-test may call this function to set
# a specific implementor for any needed implementation. FRAMEWORK_DEFAULT may be supplied to indicate a default choice
# set by the framework, which can be overridden by the platform and module selections.
#
# **IMPLEMENTATION:** implementation module name that is implemented by IMPLEMENTOR
# **IMPLEMENTOR:** implementor of IMPLEMENTATION
####
function(choose_fprime_implementation IMPLEMENTATION IMPLEMENTOR)
    resolve_dependencies(IMPLEMENTATION "${IMPLEMENTATION}")
    resolve_dependencies(IMPLEMENTOR "${IMPLEMENTOR}")
    # Check for passed in module name
    if (ARGC EQUAL 3)
        set(ACTIVE_MODULE "${ARGV2}")
    elseif (FPRIME_CURRENT_MODULE)
        set(ACTIVE_MODULE "${FPRIME_CURRENT_MODULE}")
    elseif(FPRIME_PLATFORM)
        set(ACTIVE_MODULE "${FPRIME_PLATFORM}")
    else()
        message(FATAL_ERROR "Cannot call 'choose_fprime_implementation' outside an fprime module or platform CMake file")
    endif()
    create_implementation_interface("${IMPLEMENTATION}")
    # Add this implementation in the case it has not been added
    append_list_property("${IMPLEMENTATION}" GLOBAL PROPERTY "REQUIRED_IMPLEMENTATIONS")
    set_property(GLOBAL PROPERTY "${ACTIVE_MODULE}_${IMPLEMENTATION}" "${IMPLEMENTOR}")
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
