####
# FPrime.cmake:
#
# This file is the entry point for building fprime projects, libraries. It does not setup F prime as a project, but
# rather allows the users to build against fprime, fprime libraries while taking advantage of fprime's autocoding
# support. This file includes the cmake build system setup for building like fprime.
####
include_guard()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
include(utilities)
include(options)
include(sanitizers) # Enable sanitizers if they are requested
include(required)

# Setup fprime library locations
list(REMOVE_DUPLICATES FPRIME_LIBRARY_LOCATIONS)
set(FPRIME_BUILD_LOCATIONS "${FPRIME_FRAMEWORK_PATH}" ${FPRIME_LIBRARY_LOCATIONS} "${FPRIME_PROJECT_ROOT}")
list(REMOVE_DUPLICATES FPRIME_BUILD_LOCATIONS)

# Message describing the fprime setup
message(STATUS "[FPRIME] Module locations: ${FPRIME_BUILD_LOCATIONS}")
message(STATUS "[FPRIME] Configuration module: ${FPRIME_CONFIG_DIR}")
message(STATUS "[FPRIME] Installation directory: ${CMAKE_INSTALL_PREFIX}")
include(platform/platform) # Now that module locations are known, load platform settings

# Module setup functions, attaches targets to modules, etc.
include(module)
# Support for autocoder implementations
include(autocoder/autocoder)
# Support for build target registration
include(target/target)
# Load domain-specific CMake functions
include(API)
# Sub-build support
include(sub-build/sub-build)
# C and C++ settings for building the framework
include(settings)

####
# Function `fprime_setup_global_includes`:
#
# Adds basic include directories that make fprime work. This ensures that configuration, framework, and project all
# function as expected. This will also include the internal build-cache directories.
####
function(fprime_setup_global_includes)
    # Setup the global include directories that exist outside of the build cache
    include_directories("${FPRIME_FRAMEWORK_PATH}")
    include_directories("${FPRIME_CONFIG_DIR}")
    include_directories("${FPRIME_PROJECT_ROOT}")

    # Setup the include directories that exist within the build-cache
    include_directories("${CMAKE_BINARY_DIR}")
    include_directories("${CMAKE_BINARY_DIR}/F-Prime")
    include_directories("${CMAKE_BINARY_DIR}/config")
endfunction(fprime_setup_global_includes)

####
# Function `fprime_detect_libraries`:
#
# This function detects libraries using the FPRIME_LIBRARY_LOCATIONS variable. Fore each library path, the following is
# done:
# 1. Detect a manifest file from in-order: `library.cmake`, and then `<library name>.cmake`
# 2. Add the library's top-level cmake directory to the CMAKE_MODULE_PATH
# 3. Add the library root as an include directory
# 4. Add option() to disable library UTs
####
macro(fprime_detect_libraries)
    foreach (LIBRARY_DIR IN LISTS FPRIME_LIBRARY_LOCATIONS)
        get_filename_component(LIBRARY_NAME "${LIBRARY_DIR}" NAME)
        # Detect manifest file:
        #  1. library.cmake (preferred)
        #  2. <library>.cmake (old standard)
        if (EXISTS "${LIBRARY_DIR}/library.cmake")
            set(MANIFEST_FILE "${LIBRARY_DIR}/library.cmake")
        elseif (EXISTS "${LIBRARY_DIR}/${LIBRARY_NAME}.cmake")
            set(MANIFEST_FILE "${LIBRARY_DIR}/${LIBRARY_NAME}.cmake")
        else()
            message(WARNING "[LIBRARY] ${LIBRARY_DIR} does not define library.cmake nor ${LIBRARY_NAME}.cmake. Skipping.")
            continue()
        endif()
        message(STATUS "[LIBRARY] Including library ${LIBRARY_NAME} at ${LIBRARY_DIR}")
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[LIBRARY] ${LIBRARY_NAME} using manifest ${MANIFEST_FILE}")
        endif()
        append_list_property("${MANIFEST_FILE}" GLOBAL PROPERTY FPRIME_LIBRARY_MANIFESTS)
        # Check to see if the cmake directory exists and add it
        if (IS_DIRECTORY "${LIBRARY_DIR}/cmake")
            list(APPEND CMAKE_MODULE_PATH "${LIBRARY_DIR}/cmake")
        endif()
        include_directories("${LIBRARY_DIR}")
        option(FPRIME_ENABLE_${LIBRARY_NAME}_UTS "Enable UT generation for ${LIBRARY_NAME}" ON)
    endforeach()
endmacro(fprime_detect_libraries)

####
# Function `fprime_setup_standard_targets`:
#
# Registers the targets required for a standard fprime build. This will be changed when FPRIME_SUB_BUILD_TARGETS.
####
macro(fprime_setup_standard_targets)
    # Prevent registration of standard targets when specific targets are supplied
    # This is done for efficiency to not load all the below files
    if (NOT DEFINED FPRIME_SUB_BUILD_TARGETS)
        # FPP locations must come at the front of the list, then build
        register_fprime_target(target/build)
        register_fprime_build_autocoder(autocoder/fpp)
        register_fprime_build_autocoder(autocoder/ai_xml)
        register_fprime_build_autocoder(autocoder/packets)
        register_fprime_target(target/refresh_cache)
        register_fprime_target(target/version)
        register_fprime_target(target/install)
        register_fprime_ut_target(target/ut)

        if (FPRIME_ENABLE_UTIL_TARGETS)
            register_fprime_target(target/impl)
            register_fprime_ut_target(target/check)
            register_fprime_ut_target(target/check_leak)
            register_fprime_ut_target(target/testimpl)
        endif()
    endif()
endmacro(fprime_setup_standard_targets)

####
# Function `fprime_setup_override_targets`:
#
# Override the targets that are registered by the default build with those supplied in FPRIME_SUB_BUILD_TARGETS. If
# FPRIME_SUB_BUILD_TARGETS is defined, nothing happens.
#####
macro(fprime_setup_override_targets)
    # Required to prevent overriding when not used
    if (FPRIME_SUB_BUILD_TARGETS)
        foreach (OVERRIDE_TARGET IN LISTS FPRIME_SUB_BUILD_TARGETS)
            register_fprime_target("${OVERRIDE_TARGET}")
        endforeach ()
    elseif(CMAKE_DEBUG_OUTPUT)
        message(STATUS "FPRIME_SUB_BUILD_TARGETS not defined, skipping.")
    endif()
endmacro(fprime_setup_override_targets)

macro(fprime_initialize_build_system)
    cmake_minimum_required(VERSION 3.13)
    fprime_setup_global_includes()
    fprime_detect_libraries()
    fprime_setup_standard_targets()
    fprime_setup_override_targets()
    set_property(GLOBAL PROPERTY FPRIME_BUILD_SYSTEM_LOADED ON)

    # Perform necessary sub-builds
    run_sub_build(info-cache target/fpp_locs target/fpp_depend)
endmacro(fprime_initialize_build_system)

####
# Function `fprime_setup_included_code`:
#
# Sets up the code/build for fprime and libraries. Call after all project specific targets and autocoders are set up and
# registered.
####
function(fprime_setup_included_code)
    # Must be done before code is registered but after custom target registration
    setup_global_targets()
    # For BUILD_TESTING builds then set up libraries that support testing
    if (BUILD_TESTING AND NOT DEFINED FPRIME_SUB_BUILD_TARGETS)
        add_subdirectory("${FPRIME_FRAMEWORK_PATH}/googletest/" "${CMAKE_BINARY_DIR}/F-Prime/googletest")
    endif()
    if (BUILD_TESTING)
        add_subdirectory("${FPRIME_FRAMEWORK_PATH}/STest/" "${CMAKE_BINARY_DIR}/F-Prime/STest")
    endif()
    # By default we shutoff framework UTs
    set(__FPRIME_NO_UT_GEN__ ON)
    # Check for autocoder UTs
    if (FPRIME_ENABLE_FRAMEWORK_UTS AND FPRIME_ENABLE_AUTOCODER_UTS)
        set(__FPRIME_NO_UT_GEN__ OFF)
    endif()
    add_subdirectory("${FPRIME_FRAMEWORK_PATH}/Autocoders/" "${CMAKE_BINARY_DIR}/F-Prime/Autocoders")
    # Check if we are allowing framework UTs
    if (FPRIME_ENABLE_FRAMEWORK_UTS)
        set(__FPRIME_NO_UT_GEN__ OFF)
    endif()
    # Faux libraries used as interfaces to non-autocoded fpp items
    add_library(Fpp INTERFACE)

    # Specific configuration handling
    set(FPRIME_CURRENT_MODULE config)
    add_subdirectory("${FPRIME_CONFIG_DIR}" "${CMAKE_BINARY_DIR}/config")

    set(_FP_CORE_PACKAGES Fw Svc Os Drv CFDP Utils)
    foreach (_FP_PACKAGE_DIR IN LISTS _FP_CORE_PACKAGES)
        set(FPRIME_CURRENT_MODULE "${_FP_PACKAGE_DIR}")
        add_subdirectory("${FPRIME_FRAMEWORK_PATH}/${_FP_PACKAGE_DIR}/" "${CMAKE_BINARY_DIR}/F-Prime/${_FP_PACKAGE_DIR}")
    endforeach ()
    unset(FPRIME_CURRENT_MODULE)
    get_property(FPRIME_LIBRARY_MANIFESTS GLOBAL PROPERTY FPRIME_LIBRARY_MANIFESTS)
    foreach (LIBRARY_MANIFEST IN LISTS FPRIME_LIBRARY_MANIFESTS)
        set(__FPRIME_NO_UT_GEN__ OFF)
        get_filename_component(LIB_DIR "${LIBRARY_MANIFEST}" DIRECTORY)
        get_filename_component(LIBRARY_NAME "${LIB_DIR}" NAME)
        if (FPRIME_ENABLE_${LIBRARY_NAME}_UTS)
            set(__FPRIME_NO_UT_GEN__ ON)
        endif()
        include("${LIBRARY_MANIFEST}")
    endforeach()
    # Always enable UTs for a project
    set(__FPRIME_NO_UT_GEN__ OFF)
endfunction(fprime_setup_included_code)


# Load the build system exactly one time
get_property(FPRIME_BUILD_SYSTEM_LOADED GLOBAL PROPERTY FPRIME_BUILD_SYSTEM_LOADED)
if (NOT FPRIME_BUILD_SYSTEM_LOADED)
    fprime_initialize_build_system()
endif ()
