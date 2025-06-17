####
# build.cmake:
#
# This target sets up the build for every module in the system. WARNING: it registers a target set to the module name,
# not including _build. This is for historical reasons.
####
include_guard()
include(autocoder/autocoder)
include(utilities)
include(implementation)

# Flags used when BUILD_TESTING is enabled
set(FPRIME_TESTING_REQUIRED_COMPILE_FLAGS)
set(FPRIME_TESTING_REQUIRED_LINK_FLAGS)

# Special coverage for unit tests
if (FPRIME_ENABLE_UT_COVERAGE)
    list(APPEND FPRIME_TESTING_REQUIRED_COMPILE_FLAGS -fprofile-arcs -ftest-coverage)
    list(APPEND FPRIME_TESTING_REQUIRED_LINK_FLAGS --coverage)
endif()

####
# Function `fprime__internal_TECH_DEBT_module_setup`:
#
# A function containing all the steps necessary to maintain build-system technical debt within the build target.
#
####
function(fprime__internal_TECH_DEBT_module_setup BUILD_MODULE_NAME MODULE_NAME_HELPER)
    #### Load target properties ####
    # This switches the following code to use new properties.
    foreach(PROPERTY IN ITEMS FPRIME_TYPE SOURCES SUPPLIED_SOURCES AC_GENERATED LINK_LIBRARIES SUPPLIED_HEADERS AC_FILE_DEPENDENCIES)
        get_target_property("MODULE_${PROPERTY}" "${BUILD_MODULE_NAME}" "${PROPERTY}")
        if (NOT MODULE_${PROPERTY})
            set("MODULE_${PROPERTY}")
        endif()
    endforeach()

    #### Remove empty.cpp ####
    # This section removes empty.cpp "fake source" from the various modules. This source is added to make
    # sub-builds work correctly (targets need at least one source, even if they are there just to be a name).
    # A better approach would be to add fprime_modules as "INTERFACE" targets during sub-builds thus making them
    # not require sources while still providing the name. 
    list(REMOVE_ITEM MODULE_SOURCES "${FPRIME__INTERNAL_EMPTY_CPP}")
    set_target_properties(
        ${BUILD_MODULE_NAME}
        PROPERTIES
        SOURCES "${MODULE_SOURCES}"
    )
    #### End Remove empty.cpp ####
endfunction()


function(fprime__internal_check_restrictions MODULE_NAME DEPENDENCIES)
    get_property(RESTRICTED_TARGETS GLOBAL PROPERTY "RESTRICTED_TARGETS")
    foreach(DEPENDENCY IN LISTS DEPENDENCIES)
        if (DEPENDENCY IN_LIST RESTRICTED_TARGETS)
            fprime_cmake_fatal_error("${DEPENDENCY} is not available on platform '${FPRIME_PLATFORM}' nor toolchain '${FPRIME_TOOLCHAIN}'")
        endif()
    endforeach()
endfunction()

function(fprime__internal_standard_build_target_setup BUILD_TARGET_NAME MODULE_NAME_HELPER)
    fprime__internal_TECH_DEBT_module_setup("${BUILD_TARGET_NAME}" "${MODULE_NAME_HELPER}")

    # **Must** come after the TECH_DEBT section above
    # Adds in assertion compile flags (U32 for CRC, file paths for files)
    get_target_property(BUILDABLE_SOURCES "${BUILD_TARGET_NAME}" SOURCES)
    foreach(SRC_FILE IN LISTS BUILDABLE_SOURCES)
        set_assert_flags("${SRC_FILE}")
    endforeach()

    # Check for restricted dependencies in the module's linked list
    get_target_property(TARGET_LINK_DEPENDENCIES "${BUILD_TARGET_NAME}" LINK_LIBRARIES)
    get_target_property(TARGET_INTERFACE_DEPENDENCIES "${BUILD_TARGET_NAME}" INTERFACE_LINK_LIBRARIES)
    fprime__internal_check_restrictions("${BUILD_TARGET_NAME}" "${TARGET_LINK_DEPENDENCIES};${TARGET_INTERFACE_DEPENDENCIES}")

    # Special flags applied to modules when compiling with testing enabled
    get_target_property(TARGET_TYPE "${BUILD_TARGET_NAME}" TYPE)
    if (BUILD_TESTING AND NOT TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
        target_compile_options("${BUILD_TARGET_NAME}" PRIVATE ${FPRIME_TESTING_REQUIRED_COMPILE_FLAGS})
        target_link_libraries("${BUILD_TARGET_NAME}" PRIVATE ${FPRIME_TESTING_REQUIRED_LINK_FLAGS})
    endif()
endfunction()

####
# Build function `add_global_target`:
#
# Specifically does nothing.  The "all" target of a normal cmake build will cover this case.
####
function(build_add_global_target TARGET)
endfunction(build_add_global_target)

####
# Function `add_deployment_target`:
#
# Adds in a deployment target, which for build, is just a normal module target. See: add_module_target for a description
# of arguments. FULL_DEPENDENCY_LIST is unused (these are already known to CMake).
####
function(build_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
    build_add_module_target("${MODULE}" "${TARGET}" "${SOURCES}" "${FULL_DEPENDENCY_LIST}")
endfunction()

####
# Function `build_add_module_target`:
#
# Adds a module-by-module target for building fprime.
#
# - **MODULE:** name of the module
# - **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
# - **SOURCES:** list of source file inputs from the CMakeLists.txt setup
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(build_add_module_target BUILD_TARGET_NAME TARGET SOURCES DEPENDENCIES)
    get_property(BUILD_AUTOCODERS GLOBAL PROPERTY FPRIME_AUTOCODER_TARGET_LIST)
    run_ac_set("${MODULE}" ${BUILD_AUTOCODERS})

    fprime__internal_standard_build_target_setup("${BUILD_TARGET_NAME}" "")

    # Introspection prints
    if (CMAKE_DEBUG_OUTPUT)
        introspect("${MODULE}")
    endif()
endfunction(build_add_module_target)
