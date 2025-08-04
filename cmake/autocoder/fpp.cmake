####
# autocoder/fpp.cmake:
#
# CMake implementation of an fprime autocoder. Includes the necessary function definitions to implement the fprime
# autocoder API and wraps calls to the FPP tools.
####
include_guard()
include(utilities)
include(autocoder/helpers)
set(FPRIME_FPP_TO_DICT_WRAPPER "${CMAKE_CURRENT_LIST_DIR}/scripts/fpp_to_dict_wrapper.py")

autocoder_setup_for_multiple_sources()
####
# locate_fpp_tools:
#
# Locates the fpp tool suite and sets FPP_FOUND if the right version of the tools is found. It will look first to the
# above install location and then to the system path as a fallback.
####
function(locate_fpp_tools)
    # Loop through each tool, looking if it was found and check the version
    get_expected_tool_version("fprime-fpp" FPP_VERSION)
    foreach(TOOL FPP_DEPEND FPP_TO_CPP FPP_LOCATE_DEFS FPP_TO_DICT)
        # Skipped already defined tools
        if (${TOOL})
            continue()
        endif ()
        string(TOLOWER ${TOOL} PROGRAM)
        string(REPLACE "_" "-" PROGRAM "${PROGRAM}")

        # Clear any previous version of this find and search in this order: install dir, system path
        unset(${TOOL} CACHE)
        find_program(${TOOL} ${PROGRAM})
        # If the tool exists, check the version
        if (${TOOL} AND FPRIME_SKIP_TOOLS_VERSION_CHECK)
            continue()
        elseif(${TOOL})
            set(FPP_RE_MATCH "(v[0-9]+\.[0-9]+\.[0-9]+[a-g0-9-]*)")
            execute_process(COMMAND ${${TOOL}} --help OUTPUT_VARIABLE OUTPUT_TEXT)
            if (OUTPUT_TEXT MATCHES "${FPP_RE_MATCH}")
                ends_with(ENDS_WITH_EXPECTED "${CMAKE_MATCH_1}" "${FPP_VERSION}")
                if (ENDS_WITH_EXPECTED)
                    continue()
                endif()
                message(STATUS "[fpp-tools] ${${TOOL}} version ${CMAKE_MATCH_1} not expected version ${FPP_VERSION}")
                set(FPP_REINSTALL_ERROR_MESSAGE
                    "fpp-tools version incompatible. Found ${CMAKE_MATCH_1}, expected ${FPP_VERSION}." PARENT_SCOPE
                )
            elseif(OUTPUT_TEXT MATCHES "requires 'java'")
                set(FPP_ERROR_MESSAGE
                        "fpp tools require 'java'. Please install 'java' and ensure it is on your PATH." PARENT_SCOPE
                )
            else()
                message(STATUS "[fpp-tools] ${PROGRAM} installed incorrectly.")
                set(FPP_REINSTALL_ERROR_MESSAGE "fpp tools installed incorrectly." PARENT_SCOPE)
            endif()
        else()
            message(STATUS "[fpp-tools] Could not find ${PROGRAM}.")
        endif()
        set(FPP_FOUND FALSE PARENT_SCOPE)
        return()
    endforeach()
    set(FPP_FOUND TRUE PARENT_SCOPE)
endfunction(locate_fpp_tools)

####
# Function `is_supported`:
#
# Required function, processes FPP files.
# `AC_INPUT_FILE` potential input to the autocoder
####
function(fpp_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix(".fpp" "${AC_INPUT_FILE}" TRUE)
endfunction(fpp_is_supported)

####
# Function `fpp_get_framework_dependency_helper`:
#
# Helps detect framework dependencies. Either, it calculates specific dependencies *or* if the Fw roll-up target exists,
# it will depend on that.  Note: targets within Fw always calculate the internal Fw targets as depending on Fw would
# cause a circular dependency.
#
# MODULE_NAME: current module being processed
# FRAMEWORK: list of framework dependencies. **NOTE:** will be overridden in PARENT_SCOPE with updated list
####
function(fpp_get_framework_dependency_helper MODULE_NAME FRAMEWORK)
    get_target_property(FPRIME_IS_CONFIG "${MODULE_NAME}" FPRIME_CONFIGURATION)
    # Subset the framework dependencies, or where possible use the Fw interface target
    if (FPRIME_IS_CONFIG)
        # config modules have no automatic dependencies
    elseif (NOT DEFINED FPRIME_FRAMEWORK_MODULES)
        fprime_fatal_cmake_error("${MODULE_NAME} Fw/CMakeLists.txt not included in deployment")
    elseif (MODULE_NAME STREQUAL Fw_Types)
        # Skip Fw_Types as it is the root dependency
    elseif (NOT TARGET Fw OR MODULE_NAME IN_LIST FPRIME_FRAMEWORK_MODULES)
        list(APPEND FRAMEWORK ${FPRIME_FRAMEWORK_MODULES})
        list(FIND FRAMEWORK "${MODULE_NAME}" START_INDEX)
        math(EXPR START_INDEX "${START_INDEX} + 1")
        list(SUBLIST FRAMEWORK ${START_INDEX} -1 FRAMEWORK)
    else()
        list(APPEND FRAMEWORK Fw)
    endif()
    set(FRAMEWORK "${FRAMEWORK}" PARENT_SCOPE)
endfunction(fpp_get_framework_dependency_helper)

####
# Function `fpp_info`:
#
# Given a set of supported autocoder input files, this will produce a list of files that will be generated. It sets the
# following variables in parent scope:
#
# - GENERATED_FILES: a list of files generated for the given input sources
# - MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
# - FILE_DEPENDENCIES: specific file dependencies of the given input sources
# - FPP_IMPORTS: The fpp model dependencies, which end up being the input to the -i flag for the fpp-to-cpp tool
#
# Note: although this function is only required to set `GENERATED_FILES`, the remaining information is also set as
# setting this information now will prevent a duplicated call to the tooling.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(fpp_info MODULE_NAME AC_INPUT_FILES)
    find_program(FPP_DEPEND fpp-depend)
    if (DEFINED FPP_TO_DEPEND-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    set(DIRECT_DEPENDENCIES_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/direct.txt")
    set(INCLUDED_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/include.txt")
    set(MISSING_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/missing.txt")
    set(GENERATED_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/generated.txt")
    set(FRAMEWORK_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/framework.txt")
    set(STDOUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/stdout.txt")
    set(UNITTEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/fpp-cache/unittest.txt")

    # Read files and convert to lists of dependencies. e.g. read INCLUDED_FILE file into INCLUDED variable, then process
    foreach(NAME INCLUDED MISSING GENERATED DIRECT_DEPENDENCIES FRAMEWORK STDOUT UNITTEST)
        if (NOT EXISTS "${${NAME}_FILE}")
            message(FATAL_ERROR "fpp-depend cache did not generate '${${NAME}_FILE}'")
        endif()
        file(READ "${${NAME}_FILE}" "${NAME}")
        string(STRIP "${${NAME}}" "${NAME}")
        string(REPLACE "\n" ";" "${NAME}" "${${NAME}}")
    endforeach()

    fpp_get_framework_dependency_helper("${MODULE_NAME}" "${FRAMEWORK}")

    # First assemble the generated files list
    set(GENERATED_FILES)
    foreach(LINE IN LISTS GENERATED)
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${LINE}")
    endforeach()
    set(UNITTEST_FILES)
    foreach(LINE IN LISTS UNITTEST)
        list(APPEND UNITTEST_FILES "${CMAKE_CURRENT_BINARY_DIR}/${LINE}")
    endforeach()

    # If we have missing dependencies, print and fail
    if (MISSING)
        message(WARNING "[autocode/fpp] Missing dependencies ${AC_INPUT_FILES}")
        foreach (MISS IN LISTS MISSING)
            message(WARNING "[autocode/fpp]  ${MISS}")
        endforeach()
        message(FATAL_ERROR)
    endif()

    # Module dependencies are: detected "direct" + framework dependencies - "included" files
    set(FILTERED_DIRECT_DEPENDENCIES)
    foreach(ITEM IN LISTS DIRECT_DEPENDENCIES)
        if (NOT ITEM IN_LIST INCLUDED)
            list(APPEND FILTERED_DIRECT_DEPENDENCIES "${ITEM}")
        endif()
    endforeach()
    fpp_to_modules("${MODULE_NAME}" "${FILTERED_DIRECT_DEPENDENCIES}" MODULE_DEPENDENCIES)
    list(APPEND MODULE_DEPENDENCIES ${FRAMEWORK})
    list(REMOVE_DUPLICATES MODULE_DEPENDENCIES)
    # File dependencies are any files that this depends on
    set(FILE_DEPENDENCIES ${AC_INPUT_FILES} ${INCLUDED})

    # Should have been inherited from previous call to `get_generated_files`
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
    set(UNITTEST_FILES "${UNITTEST_FILES}" PARENT_SCOPE)
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
    set(FPP_IMPORTS "${STDOUT}" PARENT_SCOPE)
endfunction(fpp_info)

####
# Function `fpp_setup_autocode`:
#
# Sets up the steps to run the autocoder and produce the files during the build. This is passed the lists generated
# in calls to `get_generated_files` and `get_dependencies`.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(fpp_setup_autocode MODULE_NAME AC_INPUT_FILES)
    if (DEFINED FPP_TO_CPP-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    fpp_info("${MODULE_NAME}" "${AC_INPUT_FILES}")
    set(CMAKE_BINARY_DIR_RESOLVED "${CMAKE_BINARY_DIR}")
    set(CMAKE_CURRENT_BINARY_DIR_RESOLVED "${CMAKE_CURRENT_BINARY_DIR}")
    resolve_path_variables(CMAKE_BINARY_DIR_RESOLVED CMAKE_CURRENT_BINARY_DIR_RESOLVED)
    string(REGEX REPLACE ";" ","  FPRIME_BUILD_LOCATIONS_COMMA_SEP "${FPRIME_BUILD_LOCATIONS}")
    string(REGEX REPLACE ";" ","  FPP_IMPORTS_COMMA_SEP "${FPP_IMPORTS}")
    set(IMPORTS)
    if (FPP_IMPORTS_COMMA_SEP)
        set(IMPORTS "-i" "${FPP_IMPORTS_COMMA_SEP}")
    endif()
    # Separate the source files into the CPP
    set(GENERATED_CPP)
    set(GENERATED_DICT)
    foreach(GENERATED IN LISTS GENERATED_FILES)
        if (GENERATED MATCHES ".*TopologyDictionary\.json")
            list(APPEND GENERATED_DICT "${GENERATED}")
        # XML outputs from FPP are discarded
        elseif(GENERATED MATCHES ".*\.xml$")
        else()
            list(APPEND GENERATED_CPP "${GENERATED}")
        endif()
    endforeach()
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/fpp-import-list" "${FPP_IMPORTS}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/fpp-source-list" "${AC_INPUT_FILES}")

    # Mark included files (.fppi) as regenerators like their .fpp parents
    foreach (INCLUDED_FILE IN LISTS FILE_DEPENDENCIES)
        requires_regeneration("${INCLUDED_FILE}")
    endforeach()

    # Add in steps for CPP generation
    if (GENERATED_CPP)
        add_custom_command(
                OUTPUT ${GENERATED_CPP}
                COMMAND ${FPP_TO_CPP} "-d" "${CMAKE_CURRENT_BINARY_DIR_RESOLVED}" ${IMPORTS} ${AC_INPUT_FILES}
                    "-p" "${FPRIME_BUILD_LOCATIONS_COMMA_SEP},${CMAKE_BINARY_DIR_RESOLVED}"
                DEPENDS ${FILE_DEPENDENCIES}
        )
    endif()
    # Add in dictionary generation
    if (GENERATED_DICT)
        set(FPRIME_JSON_VERSION_FILE "${CMAKE_BINARY_DIR}/versions/version.json")
        add_custom_command(
            OUTPUT ${GENERATED_DICT}
            COMMAND ${FPRIME_FPP_TO_DICT_WRAPPER}
                "--executable" "${FPP_TO_DICT}"
                "--cmake-bin-dir" "${CMAKE_CURRENT_BINARY_DIR}"
                "--jsonVersionFile" "${FPRIME_JSON_VERSION_FILE}"
                ${IMPORTS} ${AC_INPUT_FILES}
            DEPENDS ${FILE_DEPENDENCIES}
                    ${FPRIME_JSON_VERSION_FILE}
                    version_generate
        )
    endif()

    set(AUTOCODER_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(AUTOCODER_GENERATED_BUILD_SOURCES "${GENERATED_CPP}" PARENT_SCOPE)
    set(AUTOCODER_GENERATED_OTHER "${GENERATED_DICT}" PARENT_SCOPE)
endfunction(fpp_setup_autocode)

####
# `fpp_to_modules`:
#
# Helper function. Converts a list of files and a list of autocoder inputs into a list of module names.
#
# FILE_LIST: list of files
# OUTPUT_VAR: output variable to set with result
####
function(fpp_to_modules CURRENT_MODULE FILE_LIST OUTPUT_VAR)
    init_variables(OUTPUT_DATA)
    foreach(INCLUDE IN LISTS FILE_LIST)
        get_property(MODULE_OF_INCLUDE GLOBAL PROPERTY "FPRIME_${INCLUDE}_MODULE")
        fprime_cmake_ASSERT("File module not set in sub-build: ${INCLUDE}"
            NOT "${MODULE_OF_INCLUDE}" STREQUAL "NOTFOUND")
        # Do not add current module
        if (CURRENT_MODULE STREQUAL MODULE_OF_INCLUDE)
            continue() # Skip adding to module list
        endif()
        list(APPEND OUTPUT_DATA "${MODULE_OF_INCLUDE}")
        list(REMOVE_DUPLICATES OUTPUT_DATA)
    endforeach()
    set(${OUTPUT_VAR} "${OUTPUT_DATA}" PARENT_SCOPE)
endfunction(fpp_to_modules)
