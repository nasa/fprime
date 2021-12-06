####
# Module.cmake:
#
# This cmake file contains the functions needed to compile a module for F prime. This
# includes code for generating Enums, Serializables, Ports, Components, and Topologies.
#
# These are used as the building blocks of F prime items. This includes deployments,
# tools, and individual components.
####

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
    # CMake object type
    if (NOT DEFINED FPRIME_OBJECT_TYPE)
        set(FPRIME_OBJECT_TYPE "Deployment")
    endif()
    setup_all_deployment_targets(FPRIME_TARGET_LIST "${EXECUTABLE_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
    if (TARGET "${EXECUTABLE_NAME}" AND NOT FPRIME_FPP_LOCS_BUILD)
        add_dependencies("${EXECUTABLE_NAME}" "${EXECUTABLE_NAME}_dict")
    endif()
    # Add unit test targets to deployment
    if (BUILD_TESTING)
        setup_all_deployment_targets(FPRIME_UT_TARGET_LIST "${EXECUTABLE_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
    endif()
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
    # CMake object type
    if (NOT DEFINED FPRIME_OBJECT_TYPE)
        set(FPRIME_OBJECT_TYPE "Executable")
    endif()
    setup_all_module_targets(FPRIME_TARGET_LIST "${EXECUTABLE_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
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
  if (NOT DEFINED FPRIME_OBJECT_TYPE)
      set(FPRIME_OBJECT_TYPE "Library")
  endif()
  # Register all targets on this module, then introspect
  setup_all_module_targets(FPRIME_TARGET_LIST "${MODULE_NAME}" "${SOURCE_FILES}" "${DEPENDENCIES}")
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
function(generate_ut UT_EXE_NAME UT_SOURCES_FILE DEPENDENCIES)
    if (NOT DEFINED FPRIME_OBJECT_TYPE)
        set(FPRIME_OBJECT_TYPE "Unit Test")
    endif()
    get_module_name("${CMAKE_CURRENT_LIST_DIR}")
    setup_all_module_targets(FPRIME_UT_TARGET_LIST ${MODULE_NAME} "${UT_SOURCES_FILE}" "${DEPENDENCIES}")
endfunction(generate_ut)

