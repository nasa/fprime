####
# platforms:
#
# Platforms in F prime's CMake system setup f prime specific items w.r.t the OS. This file loads those platforms
# file and uses it to setup F prime's build. See: [Platform Template](./platform-template.md) for how to
# generate these files.
#
####
include_guard()
include(API)
include(utilities)

####
# Function `fprime_validate_platform`:
#
# This function validates the platform setting set by the toolchain file. It ensures that the toolchain file is
# correctly written. When there is not toolchain file, it sets the TOOLCHAIN_NAME and FPRIME_PLATFORM variables in
# the CMake Cache so that the system can use them.
#
# Args: None
# Returns: None
####
function(fprime_validate_platform)
    get_filename_component(TOOLCHAIN_NAME "${CMAKE_TOOLCHAIN_FILE}" NAME_WE)
    # Native toolchains use the system name for the toolchain and FPRIME_PLATFORM
    if (NOT TOOLCHAIN_NAME)
        set(TOOLCHAIN_NAME "${CMAKE_SYSTEM_NAME}")
        set(FPRIME_PLATFORM "${CMAKE_SYSTEM_NAME}" CACHE INTERNAL "Set the platform name to the system name for native builds" FORCE)
    # It is an error to use a "Generic" toolchain without setting FPRIME_PLATFORM correctly
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Generic" AND NOT FPRIME_PLATFORM)
        message(FATAL_ERROR "Toolchain '${TOOLCHAIN_NAME}' set CMAKE_SYSTEM_NAME to 'Generic' without setting FPRIME_PLATFORM")
    # It is an error to set neither of CMAKE_SYSTEM_NAME and FPRIME_PLATFORM
    elseif (NOT CMAKE_SYSTEM_NAME AND NOT FPRIME_PLATFORM)
        message(FATAL_ERROR "Toolchain '${TOOLCHAIN_NAME}' should set CMAKE_SYSTEM_NAME to 'Generic' and set FPRIME_PLATFORM")
    # Fallback to CMAKE_SYSTEM_NAME when only CMAKE_SYSTEM_NAME is set
    elseif (NOT FPRIME_PLATFORM)
        message(WARNING "Toolchain '${TOOLCHAIN_NAME}' should set CMAKE_SYSTEM_NAME to 'Generic' and set FPRIME_PLATFORM")
        set(FPRIME_PLATFORM "${CMAKE_SYSTEM_NAME}" CACHE INTERNAL "Set the platform name to the system name for random toolchain builds" FORCE)
    endif()
    # Always cache TOOLCHAIN_NAME so it is available after this function returns
    set(TOOLCHAIN_NAME "${TOOLCHAIN_NAME}" CACHE INTERNAL "Toolchain name derived from toolchain file" FORCE)
endfunction()

####
# Function `fprime_find_platform_file`:
#
# Search for a platform file (cmake/platform/${FPRIME_PLATFORM}.cmake) using ordered glob patterns.
# Each pattern group is searched individually via fprime_glob_ordered so that earlier groups take
# priority over later groups. Within a single group, matches are equally valid.
#
# Priority order:
#   1. PROJECT_SOURCE_DIR/cmake/platform/        (project-level direct)
#   2. FPRIME_PROJECT_ROOT/cmake/platform/       (backwards compat — project root differs from PROJECT_SOURCE_DIR)
#   3. PROJECT_SOURCE_DIR/lib/*/cmake/platform/  (project libraries)
#   4. PROJECT_SOURCE_DIR/*/cmake/platform/      (project subdirectories)
#   5. FPRIME_LIBRARY_LOCATIONS/cmake/platform/  (backwards compat — explicit library locations)
#   6. FPRIME_FRAMEWORK_PATH/cmake/platform/     (framework fallback)
#
# Results are cached in FPRIME_CACHED_PLATFORM_FILE to avoid re-searching.
#
# Args: None
# Returns: None
####
function(fprime_find_platform_file)
    fprime_cmake_ASSERT("FPRIME_PLATFORM changed. Please regenerate the build cache." FPRIME_PLATFORM STREQUAL FPRIME_CACHED_PLATFORM OR NOT DEFINED FPRIME_CACHED_PLATFORM)

    # Use the cached platform file to avoid the expense of glob-searching for platform files again
    if (DEFINED FPRIME_CACHED_PLATFORM_FILE)
        return()
    endif()

    # Build the ordered list of glob patterns. Each pattern is globbed individually by
    # fprime_glob_ordered (utilities.cmake) so earlier patterns take priority over later ones.
    set(_PLATFORM_GLOBS
        "${PROJECT_SOURCE_DIR}/cmake/platform/${FPRIME_PLATFORM}.cmake"
    )

    # ---- BACKWARDS COMPATIBILITY ----
    # The following patterns support older project structures that place platform files relative to
    # FPRIME_PROJECT_ROOT or FPRIME_LIBRARY_LOCATIONS rather than PROJECT_SOURCE_DIR. These exist
    # to avoid breaking existing projects during the transition to convention-based discovery.
    # TODO: Remove these patterns once all projects have migrated to the standard layout.
    if (DEFINED FPRIME_PROJECT_ROOT AND NOT "${FPRIME_PROJECT_ROOT}" STREQUAL ""
            AND NOT "${FPRIME_PROJECT_ROOT}" STREQUAL "${PROJECT_SOURCE_DIR}")
        list(APPEND _PLATFORM_GLOBS
            "${FPRIME_PROJECT_ROOT}/cmake/platform/${FPRIME_PLATFORM}.cmake"
        )
    endif()
    # ---- END BACKWARDS COMPATIBILITY (project root) ----

    list(APPEND _PLATFORM_GLOBS
        "${PROJECT_SOURCE_DIR}/lib/*/cmake/platform/${FPRIME_PLATFORM}.cmake"
        "${PROJECT_SOURCE_DIR}/*/cmake/platform/${FPRIME_PLATFORM}.cmake"
    )

    # ---- BACKWARDS COMPATIBILITY ----
    # TODO: Remove these patterns once all projects have migrated to the standard layout.
    if (DEFINED FPRIME_LIBRARY_LOCATIONS AND NOT "${FPRIME_LIBRARY_LOCATIONS}" STREQUAL "")
        foreach(LIBRARY_DIR IN LISTS FPRIME_LIBRARY_LOCATIONS)
            list(APPEND _PLATFORM_GLOBS "${LIBRARY_DIR}/cmake/platform/${FPRIME_PLATFORM}.cmake")
        endforeach()
    endif()
    # ---- END BACKWARDS COMPATIBILITY (library locations) ----

    list(APPEND _PLATFORM_GLOBS
        "${FPRIME_FRAMEWORK_PATH}/cmake/platform/${FPRIME_PLATFORM}.cmake"
    )

    fprime_glob_ordered(POSSIBLE_PLATFORM_FILES ${_PLATFORM_GLOBS})
    list(LENGTH POSSIBLE_PLATFORM_FILES NUM_POSSIBLE_PLATFORM_FILES)

    # Check if any platform file was found
    if (NUM_POSSIBLE_PLATFORM_FILES EQUAL 0)
        fprime_cmake_fatal_error("No platform config for '${FPRIME_PLATFORM}'. Please create: '${FPRIME_PLATFORM}.cmake'")
    endif()
    # Grab the first platform file found and warn if multiple were available
    list(GET POSSIBLE_PLATFORM_FILES 0 FIRST_FOUND_PLATFORM_FILE)
    if (NUM_POSSIBLE_PLATFORM_FILES GREATER 1)
        fprime_cmake_warning("Multiple '${FPRIME_PLATFORM}.cmake' files found: ${POSSIBLE_PLATFORM_FILES}. Using '${FIRST_FOUND_PLATFORM_FILE}'")
    endif()

    # Cache the results to prevent churn on re-scanning the directory
    set(FPRIME_CACHED_PLATFORM "${FPRIME_PLATFORM}" CACHE INTERNAL "Cache the platform for validation and to avoid re-searching for the platform file")
    set(FPRIME_CACHED_PLATFORM_FILE "${FIRST_FOUND_PLATFORM_FILE}" CACHE INTERNAL "Cache the platform file for validation and to avoid re-searching for the platform file")
endfunction()

####
# Macro `fprime_setup_platform`:
#
# This macro is the main entry point for setting up the platform. It validates the platform settings, finds the
# platform file, and includes it to set up the platform for the build. It also handles some left-over toolchain
# processing by setting output directories and printing the toolchain information.
#
# NOTE: this is a macro so that the included file is included in the scope of the caller.
#
# Args: None
# Returns: None
####
macro(fprime_setup_platform)
    fprime_validate_platform()
    fprime_find_platform_file()

    # Include platform file based on system name
    fprime_cmake_debug_message("Target build toolchain/platform: ${TOOLCHAIN_NAME}/${FPRIME_CACHED_PLATFORM}")
    fprime_cmake_debug_message("Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
    fprime_cmake_debug_message("Platform file: ${FPRIME_CACHED_PLATFORM_FILE}")
    include("${FPRIME_CACHED_PLATFORM_FILE}")
endmacro()

