####
# autocoder/fpp.cmake:
#
# CMake implementation of an fprime autocoder. Includes the necessary function definitions to implement the fprime
# autocoder API and wraps calls to the FPP tools.
####
include(utilities)
include(autocoder/helpers)

autocoder_setup_for_multiple_sources()
####
# locate_fpp_tools:
#
# Locates the fpp tool suite and sets FPP_FOUND if the right version of the tools is found. It will look first to the
# above install location and then to the system path as a fallback.
####
function(locate_fpp_tools)
    get_expected_tool_version("fprime-fpp" FPP_VERSION)
    # Loop through each tool, looking if it was found and check the version
    foreach(TOOL FPP_DEPEND FPP_TO_XML FPP_TO_CPP FPP_LOCATE_DEFS)
        string(TOLOWER ${TOOL} PROGRAM)
        string(REPLACE "_" "-" PROGRAM "${PROGRAM}")

        # Clear any previous version of this find and search in this order: install dir, system path
        unset(${TOOL} CACHE)
        find_program(${TOOL} ${PROGRAM})
        # If the tool exists, check the version
        if (TOOL AND FPRIME_SKIP_TOOLS_VERSION_CHECK)
            continue()
        elseif(TOOL)
            set(FPP_RE_MATCH "(v[0-9]+\.[0-9]+\.[0-9]+[a-g0-9-]*)")
            execute_process(COMMAND ${${TOOL}} --help OUTPUT_VARIABLE OUTPUT_TEXT)
            if (OUTPUT_TEXT MATCHES "${FPP_RE_MATCH}")
                ends_with(ENDS_WITH_EXPECTED "${CMAKE_MATCH_1}" "${FPP_VERSION}")
                if (ENDS_WITH_EXPECTED)
                    continue()
                endif()
                message(STATUS "[fpp-tools] ${${TOOL}} version ${CMAKE_MATCH_1} not expected version ${FPP_VERSION}")
                set(FPP_ERROR_MESSAGE
                    "fpp-tools version incompatible. Found ${CMAKE_MATCH_1}, expected ${FPP_VERSION}" PARENT_SCOPE
                )
            endif()
        endif()
        set(FPP_FOUND FALSE PARENT_SCOPE)
        return()
    endforeach()
    set(FPP_FOUND TRUE PARENT_SCOPE)
endfunction(locate_fpp_tools)

####
# Function `is_supported`:
#
# Required function, processes ComponentAi.xml files.
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
    # Subset the framework dependencies, or where possible use the Fw interface target
    if (MODULE_NAME STREQUAL "config")
        # config has no automatic dependencies
    elseif (NOT DEFINED FPRIME_FRAMEWORK_MODULES)
        message(FATAL_ERROR "Fw/CMakeLists.txt not included in deployment")
    elseif (MODULE_NAME STREQUAL Fw_Cfg)
        # Skip Fw_Cfg as it is the root dependency 
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
# - FPP_IMPORTS: The fpp model dependencies, which end up being the input to the -i flag for the fpp-to-cpp and fpp-to-xml tools
#
# Note: although this function is only required to set `GENERATED_FILES`, the remaining information is also set as
# setting this information now will prevent a duplicated call to the tooling.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(fpp_info AC_INPUT_FILES)
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

    # Read files and convert to lists of dependencies. e.g. read INCLUDED_FILE file into INCLUDED variable, then process
    foreach(NAME INCLUDED MISSING GENERATED DIRECT_DEPENDENCIES FRAMEWORK STDOUT)
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
    fpp_to_modules("${FILTERED_DIRECT_DEPENDENCIES}" MODULE_DEPENDENCIES)
    list(APPEND MODULE_DEPENDENCIES ${FRAMEWORK})
    list(REMOVE_DUPLICATES MODULE_DEPENDENCIES)
    # File dependencies are any files that this depends on
    set(FILE_DEPENDENCIES ${AC_INPUT_FILES} ${INCLUDED})

    # Should have been inherited from previous call to `get_generated_files`
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
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
function(fpp_setup_autocode AC_INPUT_FILES)
    if (DEFINED FPP_TO_XML-NOTFOUND OR DEFINED FPP_TO_CPP-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    fpp_info("${AC_INPUT_FILES}")
    string(REGEX REPLACE ";" ","  FPRIME_BUILD_LOCATIONS_COMMA_SEP "${FPRIME_BUILD_LOCATIONS}")
    string(REGEX REPLACE ";" ","  FPP_IMPORTS_COMMA_SEP "${FPP_IMPORTS}")
    set(IMPORTS)
    if (FPP_IMPORTS_COMMA_SEP)
        set(IMPORTS "-i" "${FPP_IMPORTS_COMMA_SEP}")
    endif()
    # Separate the source files into the CPP and XML steps
    set(GENERATED_AI)
    set(GENERATED_CPP)
    foreach(GENERATED IN LISTS GENERATED_FILES)
        if (GENERATED MATCHES ".*\\.xml")
            list(APPEND GENERATED_AI "${GENERATED}")
        else()
            list(APPEND GENERATED_CPP "${GENERATED}")
        endif()
    endforeach()
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/fpp-input-list" "${FILE_DEPENDENCIES}")
    # Add in steps for Ai.xml generation
    if (GENERATED_AI)
        add_custom_command(
                OUTPUT  ${GENERATED_AI}
                COMMAND ${FPP_TO_XML} "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${IMPORTS} ${AC_INPUT_FILES}
                    "-p" "${FPRIME_BUILD_LOCATIONS_COMMA_SEP}"
                DEPENDS ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
    # Add in steps for CPP generation
    if (GENERATED_CPP)
        add_custom_command(
                OUTPUT ${GENERATED_CPP}
                COMMAND ${FPP_TO_CPP} "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${IMPORTS} ${AC_INPUT_FILES}
                    "-p" "${FPRIME_BUILD_LOCATIONS_COMMA_SEP},${CMAKE_BINARY_DIR}"
                DEPENDS ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
    set(AUTOCODER_GENERATED ${GENERATED_AI} ${GENERATED_CPP})
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED}" PARENT_SCOPE)
    set(AUTOCODER_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
endfunction(fpp_setup_autocode)

####
# `fpp_to_modules`:
#
# Helper function. Converts a list of files and a list of autocoder inputs into a list of module names.
#
# FILE_LIST: list of files
# OUTPUT_VAR: output variable to set with result
####
function(fpp_to_modules FILE_LIST OUTPUT_VAR)
    init_variables(OUTPUT_DATA)
    get_module_name("${CMAKE_CURRENT_SOURCE_DIR}")
    set(CURRENT_MODULE "${MODULE_NAME}")
    foreach(INCLUDE IN LISTS FILE_LIST)
        get_module_name(${INCLUDE})
        # Here we are adding a module to the modules list if all of the following are true:
        #  1. Not present already (deduplication)
        #  2. Not the current module directory as learned by the path to the autocoder inputs
        #  3. Not withing the config directory. Config dependencies are attached to every module automatically.
        if ("${MODULE_NAME}" IN_LIST OUTPUT_DATA OR CURRENT_MODULE STREQUAL MODULE_NAME OR INCLUDE MATCHES "${FPRIME_CONFIG_DIR}/.*")
            continue() # Skip adding to module list
        endif()
        list(APPEND OUTPUT_DATA "${MODULE_NAME}")
    endforeach()
    set(${OUTPUT_VAR} "${OUTPUT_DATA}" PARENT_SCOPE)
endfunction(fpp_to_modules)
