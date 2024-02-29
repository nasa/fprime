####
# Module.cmake:
#
# This cmake file contains the functions needed to compile a module for F prime. This
# includes code for generating Enums, Serializables, Ports, Components, and Topologies.
#
# These are used as the building blocks of F prime items. This includes deployments,
# tools, and individual components.
####
include_guard()
include(target/target)
include(implementation)
set(EMPTY "${FPRIME_FRAMEWORK_PATH}/cmake/empty.cpp")

####
# Function `generate_base_module_properties`:
#
# Helper used to generate the base module properties in the system along with the core target that can be adjusted
# later.
# - **TARGET_NAME**: target name being generated
# - **SOURCE_FILES**: source files as defined by user, unfiltered. Includes autocode and source inputs.
# - **DEPENDENCIES**: dependencies as defined by user, unfiltered. Includes target names and link flags.
####
function(generate_base_module_properties TARGET_TYPE TARGET_NAME SOURCE_FILES DEPENDENCIES)
    set_property(GLOBAL PROPERTY MODULE_DETECTION TRUE)

    # Add the base elements to the system
    if (TARGET_TYPE STREQUAL "Executable" OR TARGET_TYPE STREQUAL "Deployment")
        add_executable("${TARGET_NAME}" "${EMPTY}")
    elseif(TARGET_TYPE STREQUAL "Unit Test")
        add_executable("${TARGET_NAME}" "${EMPTY}")
    elseif(TARGET_TYPE STREQUAL "Library")
        add_library("${TARGET_NAME}" "${EMPTY}")
    else()
        message(FATAL_ERROR "Module ${FPRIME_CURRENT_MODULE} cannot register object of type ${TARGET_TYPE}")
    endif()

    # Modules properties for posterity
    set_target_properties("${TARGET_NAME}" PROPERTIES FP_TYPE "${TARGET_TYPE}")
    set_property(GLOBAL APPEND PROPERTY FPRIME_MODULES "${TARGET_NAME}")

    setup_module_targets("${TARGET_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
endfunction(generate_base_module_properties)

####
# Function `generate_deployment:`
#
# Top-level executable generation. Core allows for generation of UT specifics without affecting API.
#
# - **EXECUTABLE_NAME:** name of executable to be generated.
# - **SOURCE_FILES:** source files for this executable, split into AC and normal sources
# - **DEPENDENCIES:** specified module-level dependencies
####
function(generate_deployment EXECUTABLE_NAME SOURCE_FILES DEPENDENCIES)
    generate_base_module_properties("Deployment" "${EXECUTABLE_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
endfunction(generate_deployment)
####
# Function `generate_executable:`
#
# Top-level executable generation. Core allows for generation of UT specifics without affecting API.
#
# - **EXECUTABLE_NAME:** name of executable to be generated.
# - **SOURCE_FILES:** source files for this executable, split into AC and normal sources
# - **DEPENDENCIES:** specified module-level dependencies
####
function(generate_executable EXECUTABLE_NAME SOURCE_FILES DEPENDENCIES)
    generate_base_module_properties("Executable" "${EXECUTABLE_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
endfunction(generate_executable)

####
# Function `generate_library`:
#
# Generates a library as part of F prime. This runs the AC and all the other items for the build.
# It takes SOURCE_FILES_INPUT and DEPS_INPUT, splits them up into ac sources, sources, mod deps,
# and library deps.
# - *MODULE_NAME:* module name of library to build
# - *SOURCE_FILES:* source files that will be split into AC and normal sources.
# - *DEPENDENCIES:* dependencies bound for link and cmake dependencies
#
####
function(generate_library MODULE_NAME SOURCE_FILES DEPENDENCIES)
  generate_base_module_properties("Library" "${MODULE_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
endfunction(generate_library)

####
# Function `generate_ut`:
#
# Generates a unit test as part of F prime. This runs the AC and all the other items for the build.
# It takes SOURCE_FILES_INPUT and DEPS_INPUT, splits them up into ac sources, sources, mod deps,
# and library deps.
# - *UT_EXE_NAME:* exe name of unit test to build
# - *UT_SOURCES_FILE:* source files that will be split into AC and normal sources.
# - *DEPENDENCIES:* dependencies bound for link and cmake dependencies
#
####
function(generate_ut UT_EXE_NAME UT_SOURCES_FILE UT_DEPENDENCIES)
    # Only for BUILD_TESTING
    if (BUILD_TESTING)
        get_module_name("${CMAKE_CURRENT_LIST_DIR}")
        generate_base_module_properties("Unit Test" "${UT_EXE_NAME}" "${UT_SOURCES_FILE}" "${UT_DEPENDENCIES}")
    endif()
endfunction(generate_ut)

