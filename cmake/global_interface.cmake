####
# global_interface.cmake:
#
# This file provides an internal global interface target for use in the F Prime build system. Its job is to act as a
# singular target that has all the global properties that the build system requires. These include:
#
#     0. FPRIME_LOCATIONS: list of all "root" locations (source, binary)
#     1. FPRIME_SOURCE_LOCATIONS: list of all "root" locations of the project used for inferring target names.
#     2. FPRIME_BINARY_LOCATIONS: list of all binary equivalents of FPRIME_SOURCE_LOCATIONS in the build cache.
#     3. INTERFACE_INCLUDE_DIRECTORIES: list of all include directories that apply to all targets in the build system.
#         a. These include all FPRIME_SOURCE_LOCATIONS, FPRIME_BINARY_LOCATIONS, and configuration directories
#     4. FPRIME_CHOSEN_IMPLEMENTATIONS: list of all chosen implementations for selectable build modules.
#
# The global interface target also links to all BASE_CONFIG configuration modules. This allows modules to depend on,
# and receive include directories for all base configurations.
#
# The target's name is stored in the FPRIME_GLOBAL_INTERFACE_TARGET cache variable and is available to all projects.
# Users must include this file to ensure that, should they be the first to need this target, it will be created.
#
# A global target is preferable to global properties as it has several advantages. First, it allows for transitive
# properties for things like include paths. Second, its F Prime properties can be used in generator expressions.
#
# CACHE VARIABLES:
# - FPRIME_GLOBAL_INTERFACE_TARGET: the name of the global interface target. Used when interfacing with the target
#
####
include_guard()
include(utilities)
# Create a target for use as the global interface for cross-build properties. Then create a variable to store the name.
set(FPRIME_GLOBAL_INTERFACE_TARGET "__fprime_global_interface" CACHE INTERNAL "Target name of global interface" FORCE)
if (NOT TARGET ${FPRIME_GLOBAL_INTERFACE_TARGET})
    add_library(${FPRIME_GLOBAL_INTERFACE_TARGET} INTERFACE)
endif()

####
# Function `fprime_add_location_pair`:
#
# This function adds a source location and its corresponding binary location to the global interface target. This
# includes adding the SOURCE to FPRIME_SOURCE_LOCATIONS, the BINARY to FPRIME_BINARY_LOCATIONS and updating the
# INTERFACE_INCLUDE_DIRECTORIES and FPRIME_LOCATIONS property with both locations.
#
# Arguments:
# - `SOURCE`: the source location to add
# - `BINARY`: the binary location to add
# Returns: None
####
function(fprime_add_location_pair SOURCE BINARY)
    resolve_path_variables(SOURCE BINARY)
    # First update the target properties with the appropriate locations. 
    append_list_property("${SOURCE}" TARGET "${FPRIME_GLOBAL_INTERFACE_TARGET}" PROPERTY FPRIME_SOURCE_LOCATIONS)
    append_list_property("${BINARY}" TARGET "${FPRIME_GLOBAL_INTERFACE_TARGET}" PROPERTY FPRIME_BINARY_LOCATIONS)
    append_list_property("${SOURCE}" TARGET "${FPRIME_GLOBAL_INTERFACE_TARGET}" PROPERTY FPRIME_LOCATIONS)
    append_list_property("${BINARY}" TARGET "${FPRIME_GLOBAL_INTERFACE_TARGET}" PROPERTY FPRIME_LOCATIONS)
    # Add source and binaries to the interface includes of our singular global interface target
    target_include_directories("${FPRIME_GLOBAL_INTERFACE_TARGET}" INTERFACE "${SOURCE}")
    target_include_directories("${FPRIME_GLOBAL_INTERFACE_TARGET}" INTERFACE "${BINARY}")
    # Rewrite the locations metadata files from the deduplicated properties
    get_property(_SOURCE_LOCS TARGET "${FPRIME_GLOBAL_INTERFACE_TARGET}" PROPERTY FPRIME_SOURCE_LOCATIONS)
    get_property(_BINARY_LOCS TARGET "${FPRIME_GLOBAL_INTERFACE_TARGET}" PROPERTY FPRIME_BINARY_LOCATIONS)
    get_property(_ALL_LOCS TARGET "${FPRIME_GLOBAL_INTERFACE_TARGET}" PROPERTY FPRIME_LOCATIONS)
    string(REGEX REPLACE ";" "\n" _SOURCE_LOCS_NL "${_SOURCE_LOCS}")
    string(REGEX REPLACE ";" "\n" _BINARY_LOCS_NL "${_BINARY_LOCS}")
    string(REGEX REPLACE ";" "\n" _ALL_LOCS_NL "${_ALL_LOCS}")
    file(WRITE "${CMAKE_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_SOURCE_LOCATIONS_FILE}" "${_SOURCE_LOCS_NL}\n")
    file(WRITE "${CMAKE_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_BINARY_LOCATIONS_FILE}" "${_BINARY_LOCS_NL}\n")
    file(WRITE "${CMAKE_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_ALL_LOCATIONS_FILE}" "${_ALL_LOCS_NL}\n")
endfunction(fprime_add_location_pair)

####
# Function `fprime_add_historical_locations`:
#
# This function adds the list of historical locations to the global interface target. This is done for backwards
# compatibility with older versions of F Prime. This includes the following historical locations:
# - FPRIME_PROJECT_ROOT: the root source directory of the project, from settings.ini
# - FPRIME_FRAMEWORK_PATH: the source directory of the F Prime framework, from settings.ini
# - FPRIME_LIBRARY_LOCATIONS: the list of the library locations of the project, from settings.ini
#
# > [!WARNING]
# > This function and associated variables are deprecated. New projects and code should not use these variables.
#
# Arguments: None
# Returns: None
####
function(fprime_add_historical_locations)
    if (DEFINED FPRIME_PROJECT_ROOT)
        fprime_add_location_pair("${FPRIME_PROJECT_ROOT}" "${CMAKE_BINARY_DIR}")
    endif()
    if (DEFINED FPRIME_FRAMEWORK_PATH)
        fprime_add_location_pair("${FPRIME_FRAMEWORK_PATH}" "${CMAKE_BINARY_DIR}/F-Prime")
    endif()
    foreach(LIBRARY_LOCATION IN LISTS FPRIME_LIBRARY_LOCATIONS)
        fprime_add_location_pair("${LIBRARY_LOCATION}" "${CMAKE_BINARY_DIR}")
    endforeach()
endfunction(fprime_add_historical_locations)