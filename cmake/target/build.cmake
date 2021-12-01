####
# build.cmake:
#
# This target sets up the build for every module in the system. WARNING: it registers a target set to the module name,
# not including _build. This is for historical reasons.
####
include(autocoder/autocoder)
include(utilities)

# Flags used when BUILD_TESTING is enabled
set(FPRIME_TESTING_REQUIRED_COMPILE_FLAGS)
set(FPRIME_TESTING_REQUIRED_LINK_FLAGS)

# Special coverage for unit tests
if (FPRIME_ENABLE_UT_COVERAGE)
    list(APPEND FPRIME_TESTING_REQUIRED_COMPILE_FLAGS -fprofile-arcs -ftest-coverage)
    list(APPEND FPRIME_TESTING_REQUIRED_LINK_FLAGS --coverage)
endif()

####
# Build function `add_global_target`:
#
# Specifically does nothing.  The "all" target of a normal cmake build will cover this case.
####
function(add_global_target TARGET)
endfunction(add_global_target)

####
# setup_build_module:
#
# Helper function to setup the module. This was the historical core of the CMake system, now embedded as part of this
# build target. It adds a the target (library, executable), sets up compiler source files, flags generated sources,
# sets up module and linker dependencies adds the file to the hashes.txt file, sets up include directories, etc.
#
# - MODULE: module name being setup
# - SOURCES: hand-specified source files
# - GENERATED: generated sources
# - EXCLUDED_SOURCES: sources already "consumed", that is, processed by an autocoder
# - DEPENDENCIES: dependencies of this module. Also link flags and libraries.
####
function(setup_build_module MODULE SOURCES GENERATED EXCLUDED_SOURCES DEPENDENCIES)
    # Compilable sources
    set(COMPILE_SOURCES)
    foreach(SOURCE IN LISTS SOURCES GENERATED)
        if (NOT SOURCE IN_LIST EXCLUDED_SOURCES)
            list(APPEND COMPILE_SOURCES "${SOURCE}")
        endif()
    endforeach()
    # Setup the actual target
    if (FPRIME_OBJECT_TYPE STREQUAL "Library")
        # Add the library name
        add_library(${MODULE} ${COMPILE_SOURCES})
    else()
        add_executable(${MODULE} ${COMPILE_SOURCES})
    endif()

    # Set those files as generated to prevent build errors
    foreach(SOURCE IN LISTS GENERATED)
        set_source_files_properties(${SOURCE} PROPERTIES GENERATED TRUE)
    endforeach()
    # Setup the hash file for our sources
    foreach(SRC_FILE ${COMPILE_SOURCES})
        set_hash_flag("${SRC_FILE}")
    endforeach()

    # Includes the source, so that the Ac files can include source headers
    target_include_directories("${MODULE}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    # For every detected dependency, add them to the supplied module. This enforces build order.
    # Also set the link dependencies on this module. CMake rolls-up link dependencies, and thus
    # this prevents the need for manually specifying link orders.
    set(TARGET_DEPENDENCIES)
    foreach(DEPENDENCY ${DEPENDENCIES})
        if (NOT DEPENDENCY MATCHES "^-l.*")
            add_dependencies(${MODULE} "${DEPENDENCY}")
            list(APPEND TARGET_DEPENDENCIES "${DEPENDENCY}")
        endif()
        target_link_libraries(${MODULE} PUBLIC "${DEPENDENCY}")
    endforeach()
    set_property(TARGET "${MODULE}" PROPERTY FPRIME_TARGET_DEPENDENCIES ${TARGET_DEPENDENCIES})
endfunction()

####
# Function `add_deployment_target`:
#
# Adds in a deployment target, which for build, is just a normal module target. See: add_module_target for a description
# of arguments. FULL_DEPENDENCY_LIST is unused (these are already known to CMake).
####
function(add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
    add_module_target("${MODULE}" "${TARGET}" "${SOURCES}" "${DEPENDENCIES}")
endfunction()

####
# Build function `add_module_target`:
#
# Adds a module-by-module target for building fprime.
#
# - **MODULE:** name of the module
# - **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
# - **SOURCES:** list of source file inputs from the CMakeLists.txt setup
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    message(STATUS "Adding ${FPRIME_OBJECT_TYPE}: ${MODULE}")
    run_ac_set("${SOURCES}" autocoder/fpp autocoder/ai-xml)
    resolve_dependencies(RESOLVED ${DEPENDENCIES} ${AC_DEPENDENCIES} )
    setup_build_module("${MODULE}" "${SOURCES}" "${AC_GENERATED}" "${AC_SOURCES}" "${RESOLVED}")
    # Special flags applied to modules when compiling with testing enabled
    if (BUILD_TESTING)
        target_compile_options("${MODULE}" PRIVATE ${FPRIME_TESTING_REQUIRED_COMPILE_FLAGS})
        target_link_libraries("${MODULE}" PRIVATE ${FPRIME_TESTING_REQUIRED_LINK_FLAGS})
    endif()

    if (CMAKE_DEBUG_OUTPUT)
        introspect("${MODULE}")
    endif()
endfunction(add_module_target)
