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
include(target/ut) # For FPRIME__INTERNAL_UT_TARGET variable
include(utilities)

set(FPRIME__INTERNAL_EMPTY_CPP "${FPRIME_FRAMEWORK_PATH}/cmake/empty.cpp")
set(VALID_EMPTY "${FPRIME_FRAMEWORK_PATH}/cmake/valid-empty.cpp")

####
# Function `fprime__internal_add_build_target`:
#
# Processes the module arguments and set variables, then adds a build target with target properties set.
#
# Target properties set:
# - SOURCES: source files (C++ build)
# - LINK_LIBRARIES: link dependencies (C++ build) 
# - SUPPLIED_HEADERS: headers (for code counting)
# - SUPPLIED_SOURCES: original SOURCES before autocoding attached
# - SUPPLIED_DEPENDENCIES: original link dependencies before autocoding attached
# - AUTOCODER_INPUTS: input source file list for autocoding
# - FPRIME_TYPE: set to ${BUILD_TARGET_TYPE_STRING}
#
# This function sets "INTERNAL_MODULE_NAME" in PARENT_SCOPE to pass-back module name for target
# registration.
#
# - **BUILD_TARGET_TYPE_STRING:** "Library", "Executable", "Deployment", and "Unit Test".
# - **EXTRA_CONTROL_DIRECTIVES:** extra CMake `add_*` arguments (e.g. INTERFACE for interface libraries)
####
function(fprime__internal_add_build_target BUILD_TARGET_TYPE_STRING EXTRA_CONTROL_DIRECTIVES)
    fprime__process_module_setup("${BUILD_TARGET_TYPE_STRING}" "${EXTRA_CONTROL_DIRECTIVES}" ${ARGN})
    fprime__internal_add_build_target_helper("${INTERNAL_MODULE_NAME}" "${BUILD_TARGET_TYPE_STRING}" "${INTERNAL_SOURCES}" "${INTERNAL_AUTOCODER_INPUTS}" "${INTERNAL_HEADERS}" "${INTERNAL_DEPENDS}" "${INTERNAL_EXCLUDE_FROM_ALL}")
    set(INTERNAL_MODULE_NAME "${INTERNAL_MODULE_NAME}" PARENT_SCOPE)
endfunction()

####
# Function `fprime__process_module_setup`:
#
# This function is used to process the module setup. It takes a list of arguments and sorts them into
# SOURCES, HEADERS, and DEPENDS. It also sets the module name based on the first argument or the
# FPRIME_CURRENT_MODULE variable. If neither is provided, it will throw an error.
#
# It handles the backwards compatibility with the old structure where users set SOURCE_FILES, MOD_DEPS,
# etc. variables.
#
# - **FPRIME_MODULE_TYPE**: fprime type of module
# - **ADDITIONAL_CONTROL_SETS**: additional control directives to support (e.g. INTERFACE for libraries)
# - **ARGN**: list of arguments to process.
####
function(fprime__process_module_setup FPRIME_MODULE_TYPE ADDITIONAL_CONTROL_SETS)
    # Initial setup
    set(INPUT_ARGUMENTS ${ARGN})
    list(GET INPUT_ARGUMENTS 0 FIRST_ARGUMENT)
    list(LENGTH INPUT_ARGUMENTS INPUT_COUNT)

    # List of control words
    set(CONTROL_SETS "HEADERS" "SOURCES" "DEPENDS" "EXCLUDE_FROM_ALL" "AUTOCODER_INPUTS" ${ADDITIONAL_CONTROL_SETS})
    set(FILE_CONTROL_SETS "HEADERS" "SOURCES" "AUTOCODER_INPUTS")
    # Set module name as passed in, then defaulting to FPRIME_CURRENT_MODULE
    if (${INPUT_COUNT} GREATER 0 AND NOT FIRST_ARGUMENT IN_LIST CONTROL_SETS)
        list(POP_FRONT INPUT_ARGUMENTS MODULE_NAME)
    elseif(DEFINED FPRIME_CURRENT_MODULE AND FPRIME_MODULE_TYPE STREQUAL "Unit Test")
        set(MODULE_NAME ${FPRIME_CURRENT_MODULE}_${FPRIME__INTERNAL_UT_TARGET})
    elseif(DEFINED FPRIME_CURRENT_MODULE)
        set(MODULE_NAME ${FPRIME_CURRENT_MODULE})
    else()
        fprime_cmake_fatal_error("FPRIME_CURRENT_MODULE not defined. Please supply name to: register_fprime_module(<module name>)")
    endif()
    list(LENGTH INPUT_ARGUMENTS INPUT_COUNT)

    # Support the old structure where SOURCE_FILES and MOD_DEPS were set to specify module lists
    if (INPUT_COUNT EQUAL 0 AND NOT DEFINED SOURCE_FILES AND NOT FPRIME_MODULE_TYPE STREQUAL "Unit Test")
        fprime_cmake_fatal_error("Must supply SOURCES to register_fprime_*")
    elseif (INPUT_COUNT EQUAL 0 AND NOT DEFINED UT_SOURCE_FILES AND FPRIME_MODULE_TYPE STREQUAL "Unit Test")
        fprime_cmake_fatal_error("Must supply SOURCES to register_fprime_ut")
    elseif (INPUT_COUNT EQUAL 0 AND FPRIME_MODULE_TYPE STREQUAL "Unit Test" AND DEFINED UT_SOURCE_FILES)
        # C/CPP/ASM files end with "c", "cpp", "cc", "cxx", "S", "asm". SOURCES are C/CPP/ASM matching SOURCE_FILES and
        # AUTOCODER_INPUTS are non-matching SOURCE_FILES.
        sort_buildable_from_non_buildable_sources(SOURCES AUTOCODER_INPUTS "${UT_SOURCE_FILES}")
        set(HEADERS "${UT_HEADER_FILES}")
        resolve_dependencies(MOD_DEPS_RESOLVED ${UT_MOD_DEPS})
        set(DEPENDS "${MOD_DEPS_RESOLVED}")
        # Historically, the current module was added automatically to the UT dependencies 
        if (TARGET "${FPRIME_CURRENT_MODULE}")
            # Ensure the current module build target is linkable
            get_target_property(CURRENT_TARGET_TYPE "${FPRIME_CURRENT_MODULE}" TYPE)
            if (CURRENT_TARGET_TYPE MATCHES "[A-Z]*_LIBRARY")
                list(APPEND DEPENDS "${FPRIME_CURRENT_MODULE}")
            endif()
        endif()
    elseif (INPUT_COUNT EQUAL 0 AND DEFINED SOURCE_FILES)
        # C/CPP/ASM files end with "c", "cpp", "cc", "cxx", "S", "asm". SOURCES are C/CPP/ASM matching SOURCE_FILES and
        # AUTOCODER_INPUTS are non-matching SOURCE_FILES.
        sort_buildable_from_non_buildable_sources(SOURCES AUTOCODER_INPUTS "${SOURCE_FILES}")
        set(HEADERS "${HEADER_FILES}")
        resolve_dependencies(MOD_DEPS_RESOLVED ${MOD_DEPS})
        set(DEPENDS "${MOD_DEPS_RESOLVED}")
    # Check other definitions
    elseif (DEFINED SOURCE_FILES)
        fprime_cmake_fatal_error("Cannot both set SOURCE_FILES and supply source list to register_fprime_module")
    elseif (DEFINED MOD_DEPS)
        fprime_cmake_fatal_error("Cannot both set MOD_DEPS and supply a dependency list to register_fprime_module")
    elseif (DEFINED HEADER_FILES)
        fprime_cmake_fatal_error("Cannot both set HEADER_FILES and supply a dependency list to register_fprime_module")
    else()
        # Unset all the control lists so the module can track what controls were passed in along with their arguments
        # allowing signal control sets that do not take arguments.
        foreach(CONTROL_SET IN LISTS CONTROL_SETS)
            unset("${CONTROL_SET}")
        endforeach()
    endif()
    unset(CURRENT_LIST_NAME)
    # Process all arguments and fill in the module sources
    foreach (ARGUMENT IN LISTS INPUT_ARGUMENTS)
        # EXISTS only defined for resolved absolute paths
        set(RESOLVED_ARGUMENT "${ARGUMENT}")
        resolve_path_variables(RESOLVED_ARGUMENT)
        # If the argument is one of our control tokens, and the list is already defined, this means the user has specified
        # the argument twice. This is likely an error.
        if (ARGUMENT IN_LIST CONTROL_SETS AND DEFINED "${ARGUMENT}")
            fprime_cmake_fatal_error("${ARGUMENT} supplied multiple times in call to register_fprime_module")
        # Now update the current list and define the backing store for it. This will allow us to capture arguments
        # between this and other control words.
        elseif(ARGUMENT IN_LIST CONTROL_SETS)
            # Check for control words that are zero-argument (flags) and set them to true
            if (DEFINED CURRENT_LIST_NAME AND NOT DEFINED ${CURRENT_LIST_NAME})
                set("${CURRENT_LIST_NAME}" TRUE)
            endif()
            set(CURRENT_LIST_NAME "${ARGUMENT}")
            set("${CURRENT_LIST_NAME}")
        # Check that file types' files exist
        elseif(DEFINED CURRENT_LIST_NAME AND CURRENT_LIST_NAME IN_LIST FILE_CONTROL_SETS AND NOT EXISTS "${RESOLVED_ARGUMENT}")
            fprime_cmake_fatal_error("${ARGUMENT} does not exist but was specified as a SOURCE/HEADER/AUTOCODER_INPUT")
        # Add in an element to the active control list
        elseif(DEFINED CURRENT_LIST_NAME)
            list(APPEND "${CURRENT_LIST_NAME}" "${ARGUMENT}")
        # Handle arguments supplied before any control word
        else()
            string(REPLACE ";" " " CONTROL_SETS_STRING "${CONTROL_SETS}")
            fprime_cmake_fatal_error("One of ${CONTROL_SETS_STRING} must be specified before list elements: ${ARGUMENT}")
        endif()
    endforeach()
    # Check for control words that are zero-argument (flags) and set them to true
    if (DEFINED CURRENT_LIST_NAME AND NOT DEFINED ${CURRENT_LIST_NAME})
        set("${CURRENT_LIST_NAME}" TRUE)
    endif()
    # Update caller scope with the new variables
    set(INTERNAL_MODULE_NAME "${MODULE_NAME}" PARENT_SCOPE)
    foreach(CONTROL_SET IN LISTS CONTROL_SETS)
        # Define listed argument in parent scope only when they were defined within this file. This will unused control
        # words to be undefined lists in parent scope distinguishing them from empty words.
        if (DEFINED "${CONTROL_SET}")
            # FPP, Python, and other non-native (virtualized) tooling deal in absolute resolved paths. This is a function
            # of how the virtual machines underpinning these technologies work.
            #
            # Thus to make life easier on tool developers, we automatically resolve all paths as part of the interface
            # ensuring that this step is not required on each tool integration.
            resolve_path_variables(${CONTROL_SET})
            set(INTERNAL_${CONTROL_SET} "${${CONTROL_SET}}" PARENT_SCOPE)
        endif()
    endforeach(CONTROL_SET IN LISTS CONTROL_SETS)
    clear_historical_variables(PARENT_SCOPE)
endfunction()

####
# Function `fprime__internal_add_build_target_helper`:
#
# Helper to add the target and set target properties.
#
# Target properties set:
# - SOURCES: source files (C++ build)
# - LINK_LIBRARIES: link dependencies (C++ build) 
# - SUPPLIED_HEADERS: headers (for code counting)
# - SUPPLIED_SOURCES: original SOURCES before autocoding attached
# - SUPPLIED_DEPENDENCIES: original link dependencies before autocoding attached
# - AUTOCODER_INPUTS: input source file list for autocoding
# - FPRIME_TYPE: set to ${BUILD_TARGET_TYPE_STRING}
#
# - **BUILD_TARGET_TYPE_STRING:** "Library", "Executable", "Deployment", and "Unit Test".
# - **EXTRA_CONTROL_DIRECTIVES:** extra CMake `add_*` arguments (e.g. INTERFACE for interface libraries)
####
function(fprime__internal_add_build_target_helper TARGET_NAME TYPE SOURCES AUTOCODER_INPUTS HEADERS DEPENDENCIES EXTRA_CMAKE_DIRECTIVES)
    # Historical status message for posterity...and to prevent panic amongst users
    message(STATUS "Adding ${TYPE}: ${TARGET_NAME}")
    # Remap F Prime target type to CMake targe type
    if (TYPE STREQUAL "Executable" OR TYPE STREQUAL "Deployment" OR TYPE STREQUAL "Unit Test")
        add_executable("${TARGET_NAME}" ${EXTRA_CMAKE_DIRECTIVES} "${SOURCES}")
    elseif(TYPE STREQUAL "Library")
        add_library("${TARGET_NAME}" ${EXTRA_CMAKE_DIRECTIVES} ${SOURCES})
    else()
        fprime_cmake_fatal_error("Cannot register compilation target of type ${TYPE}")
    endif()
    # Use the appropriate link type for the target
    get_target_property(CMAKE_LIBRARY_TYPE "${TARGET_NAME}" TYPE)
    if (CMAKE_LIBRARY_TYPE MATCHES "INTERFACE_LIBRARY")
        target_link_libraries("${TARGET_NAME}" INTERFACE ${DEPENDENCIES})
    else()
        # TODO: this is needed because sub-builds still attempt register targets, but without the build target to add back in the
        #       autocoding output. Thus empty must be substituted. Would it be possible to force the library to be an INTERFACE
        #       instead?  Or only add empty on sub-builds?
        target_sources("${TARGET_NAME}" PRIVATE "${FPRIME__INTERNAL_EMPTY_CPP}")
        target_link_libraries("${TARGET_NAME}" PUBLIC ${DEPENDENCIES})
    endif()
    set_target_properties("${TARGET_NAME}"
        PROPERTIES
            SUPPLIED_HEADERS "${HEADERS}"
            SUPPLIED_SOURCES "${SOURCES}"
            SUPPLIED_DEPENDENCIES "${DEPENDENCIES}"
            SUPPLIED_AUTOCODER_INPUTS "${AUTOCODER_INPUTS}"
            FPRIME_DEPENDENCIES "${DEPENDENCIES}"
            AUTOCODER_INPUTS "${AUTOCODER_INPUTS}"
            FPRIME_TYPE "${TYPE}"
    )

    # System-wide properties
    set_property(GLOBAL PROPERTY MODULE_DETECTION TRUE)
    set_property(GLOBAL APPEND PROPERTY FPRIME_MODULES "${TARGET_NAME}")
    fprime_cmake_ASSERT("Target (${TARGET_NAME}) not defined" TARGET "${TARGET_NAME}")
endfunction()

