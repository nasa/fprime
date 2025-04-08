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

function(fprime__internal_TECH_DEBT_module_setup BUILD_MODULE_NAME GENERATED_SOURCES AC_DEPENDENCIES AC_FILE_DEPENDENCIES MODULE_NAME_HELPER)
    #### Missing and Incomplete Properties Patch ####
    # Patch in missing properties that **should** have been set by the autocoder. This sets the generated sources, original
    # supplied sources, file dependencies, and updates link libraries and sources.
    get_target_property("MODULE_SUPPLIED_SOURCES" "${BUILD_MODULE_NAME}" SOURCES)
    filter_lists("${AC_SOURCES}" NORMAL_SOURCES AC_GENERATED)
    set_target_properties("${BUILD_MODULE_NAME}" PROPERTIES
        SUPPLIED_SOURCES "${MODULE_SUPPLIED_SOURCES}"
        GENERATED_SOURCES "${AC_GENERATED}"
        AC_FILE_DEPENDENCIES "${AC_FILE_DEPENDENCIES}"
    )
    target_link_libraries("${BUILD_MODULE_NAME}" PUBLIC ${AC_DEPENDENCIES})
    target_sources("${BUILD_MODULE_NAME}" PRIVATE ${GENERATED_SOURCES})
    #### End Missing and Incomplete Properties Patch ####
    

    #### Load target properties ####
    # This switches the following code to use new properties.
    foreach(PROPERTY IN ITEMS FP_TYPE SUPPLIED_SOURCES GENERATED_SOURCES LINK_LIBRARIES HEADERS)
        get_target_property("MODULE_${PROPERTY}" "${BUILD_MODULE_NAME}" "${PROPERTY}")
        if (NOT MODULE_${PROPERTY})
            set("MODULE_${PROPERTY}")
        endif()
    endforeach()

    #### Create module-info.txt ####
    # module-info.txt is used as a cache to enable build system quasi-dependent tools to work as expected.
    # However, this really ought to be implemented as an autocoder itself, as it depends on nothing other
    # than the input sources, and previous autocoder runs.
    #
    # To do this correctly, headers should be passed to autocoders **and** autocoders would need to be
    # registered in-between this "last autocoder" and the other pre-autocoders.
    #
    # Create lists of hand-coded and generated sources. This should be handled in the autocoder sub-system
    # and not pushed to the build module.
    #
    # HEADER_FILES should not be read from a variable.
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/module${MODULE_NAME_HELPER}-info.txt"
        "${MODULE_HEADERS}\n${MODULE_SUPPLIED_SOURCES}\n${MODULE_GENERATED_SOURCES}\n${MODULE_AC_FILE_DEPENDENCIES}\n${MODULE_DEPENDENCIES}\n"
    )
    #### End module-info.txt ####

    #### Add Generated Sources ####
    # This section adds generated sources to the module. This is TECH_DEBT that should eventually
    # be replaced by functionality in the autocoders themselves. The autocoders know what is generated
    # and what is not generated. They should update the target_source property directly.

    # Set those files as generated to prevent build errors
    foreach(SOURCE IN LISTS MODULE_GENERATED_SOURCES)
        set_source_files_properties(${SOURCE} PROPERTIES GENERATED TRUE)
    endforeach()
    #### End Add Generated Sources ####

    #### Remove empty.cpp ####
    # This section removes empty.cpp "fake source" from the various modules. This source is added to make
    # sub-builds work correctly (targets need at least one source, even if they are there just to be a name).
    # A better approach would be to add fprime_modules as "INTERFACE" targets during sub-builds thus making them
    # not require sources while still providing the name. 
    list(REMOVE_ITEM MODULE_SOURCES "${FPRIME__INTERNAL_EMPTY_CPP}")
    set_target_properties(
        ${BUILD_MODULE_NAME}
        PROPERTIES
        SOURCES "${MODULE_SOURCES};${AC_GENERATED}"
    )
    #### End Remove empty.cpp ####

    #### Set FPRIME_TARGET_DEPENDENCIES ####
    # Can this be removed or simplified?  It is used for recursing dependencies. Perhaps we can do this with
    # add_dependencies and recursive targets instead? 
    list(REMOVE_DUPLICATES MODULE_DEPENDENCIES)
    list(FILTER MODULE_DEPENDENCIES EXCLUDE REGEX "^-.*")
    set_property(TARGET "${BUILD_MODULE_NAME}" PROPERTY FPRIME_TARGET_DEPENDENCIES ${MODULE_DEPENDENCIES})
    #### End set FPRIME_TARGET_DEPENDENCIES ####

    #### Set Implementation Choices ####
    # Extra source files, dependencies, and link libraries need to be added to executables to account for the chosen
    # implementations. First, for modules whose names differ from FPRIME_CURRENT_MODULE the chosen implementation is
    # remapped to them. Then the implementation set are calculated and sources, link libraries and dependencies added.
    #
    # This should be done per-target using IMPLEMENTATION_OVERRIDES supplied to API registration call and should not
    # be set to the current module.
    if (NOT ${MODULE_TYPE} STREQUAL "Library")
        # Handle updates when the types have diverged
        if (NOT MODULE STREQUAL "${FPRIME_CURRENT_MODULE}")
            # Update implementation choices
            remap_implementation_choices("${FPRIME_CURRENT_MODULE}" "${BUILD_MODULE_NAME}")
        endif()
        setup_executable_implementations("${BUILD_MODULE_NAME}")
    endif ()
    #### End Implementation Choices ####
endfunction()


function(fprime__internal_check_restrictions MODULE_NAME DEPENDENCIES)
    get_property(RESTRICTED_TARGETS GLOBAL PROPERTY "RESTRICTED_TARGETS")
    foreach(DEPENDENCY IN LISTS DEPENDENCIES)
        if (DEPENDENCY IN_LIST RESTRICTED_TARGETS)
            fprime_fatal_cmake_error("${DEPENDENCY} is not available on platform '${FPRIME_PLATFORM}' nor toolchain '${FPRIME_TOOLCHAIN}'")
        endif()
    endforeach()
endfunction()

function(fprime__internal_standard_module_setup MODULE SUPPLIED_SOURCES GENERATED_SOURCES DEPENDENCIES AC_FILE_DEPENDENCIES MODULE_NAME_HELPER)
    fprime__internal_TECH_DEBT_module_setup("${MODULE}" "${SUPPLIED_SOURCES}" "${GENERATED_SOURCES}" "${DEPENDENCIES}" "${AC_FILE_DEPENDENCIES}" "${MODULE_NAME_HELPER}")

    # **Must** come after the TECH_DEBT section above
    # Adds in assertion compile flags (U32 for CRC, file paths for files)
    foreach(SRC_FILE IN LISTS SUPPLIED_SOURCES GENERATED_SOURCES)
        set_assert_flags("${SRC_FILE}")
    endforeach()

    # Check for restricted dependencies in the module's linked list
    get_target_property(TARGET_LINK_DEPENDENCIES "${MODULE}" LINK_LIBRARIES)
    fprime__internal_check_restrictions("${MODULE}" "${TARGET_LINK_DEPENDENCIES}")

    # Special flags applied to modules when compiling with testing enabled
    if (BUILD_TESTING)
        target_compile_options("${MODULE}" PRIVATE ${FPRIME_TESTING_REQUIRED_COMPILE_FLAGS})
        target_link_libraries("${MODULE}" PRIVATE ${FPRIME_TESTING_REQUIRED_LINK_FLAGS})
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
function(build_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    # Assert the pre-conditions of the autocoder run
    fprime_cmake_ASSERT(NOT DEFINED AC_SOURCES)
    fprime_cmake_ASSERT(NOT DEFINED AC_GENERATED)
    fprime_cmake_ASSERT(NOT DEFINED AC_FILE_DEPENDENCIES)
    fprime_cmake_ASSERT(NOT DEFINED AC_DEPENDENCIES)
    get_property(BUILD_AUTOCODERS GLOBAL PROPERTY FPRIME_AUTOCODER_TARGET_LIST)
    run_ac_set("${MODULE}" "${SOURCES}" ${BUILD_AUTOCODERS})
    # Assert the post-conditions of the autocoder run
    fprime_cmake_ASSERT(DEFINED AC_SOURCES)
    fprime_cmake_ASSERT(DEFINED AC_GENERATED) 
    fprime_cmake_ASSERT(DEFINED AC_FILE_DEPENDENCIES)
    fprime_cmake_ASSERT(DEFINED AC_DEPENDENCIES)
    set(DEPENDENCIES ${DEPENDENCIES} ${AC_DEPENDENCIES})
    list(REMOVE_DUPLICATES DEPENDENCIES)

    fprime__internal_standard_module_setup("${MODULE}" "${SOURCES}" "${AC_SOURCES}" "${DEPENDENCIES}" "${AC_FILE_DEPENDENCIES}" "")

    # Introspection prints
    if (CMAKE_DEBUG_OUTPUT)
        introspect("${MODULE}")
    endif()
endfunction(build_add_module_target)
