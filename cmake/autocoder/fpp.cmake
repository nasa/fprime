####
# autocoder/fpp.cmake:
#
# CMake implementation of an fprime autocoder. Includes the necessary function definitions to implement the fprime
# autocoder API and wraps calls to the FPP tools.
####
include(utilities)
include(autocoder/default)

# Does not handle source files one-by-one, but as a complete set
set(HANDLES_INDIVIDUAL_SOURCES FALSE)
set(FPP_RUN_OR_REMOVE "${CMAKE_CURRENT_LIST_DIR}/fpp-wrapper/fpp-run-or-remove")
set(FPP_FRAMEWORK_DEFAULT_DEPS Fw_Prm Fw_Cmd Fw_Log Fw_Tlm Fw_Com Fw_Time Fw_Port Fw_Types Fw_Cfg)

####
# `is_supported`:
#
# Given a single input file, determines if that input file is processed by this autocoder. Sets the variable named
# IS_SUPPORTED in parent scope to be TRUE if FPP can process the given file or FALSE otherwise.
#
# AC_INPUT_FILE: filepath for consideration
####
function(is_supported AC_INPUT_FILE)
    set(IS_SUPPORTED FALSE PARENT_SCOPE)
    if (AC_INPUT_FILE MATCHES ".*.fpp")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    endif()
endfunction(is_supported)

####
# `regenrate_memo`:
#
# Determines if the memo file used by the autocoder system needs to be regenerated, or if the memoization can be read
# as-is.  Sets variable named with OUTPUT to TRUE or FALSE in parent scope.

# OUTPUT: name of variable to set in parent scope. Will be set to TRUE or FALSE.
# MEMO_FILE: path to memo file in question
# SOURCES_INPUT: list of sources used to generate the given memo file
####
function(regenerate_memo OUTPUT MEMO_FILE SOURCES_INPUT)
    # Initialize variables
    set(${OUTPUT} FALSE PARENT_SCOPE)
    set(LOCS_MOVED FALSE)
    set(OUTPUT_TEXT "n/a")
    set(ALL_FPP_INPUTS)
    set(PREV_LOCS_FILE "${MEMO_FILE}.locs.prev")
    default_regenerate_helper(MEMO_MISSING CHANGED "${MEMO_FILE}" "${SOURCES_INPUT}")

    # Read the memo file for dependencies, should it exist
    if (NOT MEMO_MISSING)
        file(READ "${MEMO_FILE}" CONTENTS)
        read_from_lines("${CONTENTS}" GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES ALL_FPP_INPUTS)
    endif()
    set_property(GLOBAL PROPERTY "FPP_LOCATIONS_CACHE_${MODULE_NAME}" "${PREV_LOCS_FILE}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${PREV_LOCS_FILE}")
    # Look at changed inter-module dependencies
    if (ALL_FPP_INPUTS)
        # Subset module deps by what changed
        string(REPLACE ";" ":" FPRIME_BUILD_LOCATIONS_SEP "${FPRIME_BUILD_LOCATIONS}")
        find_program(PYTHON NAMES python3 python REQUIRED)
        execute_process(COMMAND ${CMAKE_COMMAND} -E env 
            PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils
            BUILD_ROOT=${FPRIME_BUILD_LOCATIONS_SEP}:${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/F-Prime
            ${PYTHON} "${FPRIME_FRAMEWORK_PATH}/cmake/autocoder/fpp-locs-differ/fpp-locs-differ.py" ${FPP_LOCS_FILE} ${PREV_LOCS_FILE}
            ${ALL_FPP_INPUTS} OUTPUT_VARIABLE OUTPUT_TEXT OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE RESULT_OUT)
        if (NOT RESULT_OUT EQUAL "0")
            set(LOCS_MOVED TRUE)
        endif()
    endif()
    # Regenerating on any of the above
    if (MEMO_MISSING OR CHANGED OR LOCS_MOVED)
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FPP_LOCS_FILE} ${PREV_LOCS_FILE}) 
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] Regenerating memo '${MEMO_FILE}' because: (memo missing: ${MEMO_MISSING}, sources changed: ${CHANGED}, dependencies moved: ${LOCS_MOVED} (${OUTPUT_TEXT}))")
        endif()
	    set(${OUTPUT} TRUE PARENT_SCOPE)
    endif()
endfunction(regenerate_memo)

####
# `get_generated_files`:
#
# Given a set of supported autocoder input files, this will produce a list of files that will be generated. It sets the
# following variables in parent scope:
#
# - GENERATED_FILES: a list of files generated for the given input sources
# - MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
# - FILE_DEPENDENCIES: specific file dependencies of the given input sources
# - EXTRAS: used to publish the 'imported' file dependencies of the given input files
#
# Note: although this function is only required to set `GENERATED_FILES`, the remaining information is also set as
# setting this information now will prevent a duplicated call to the tooling.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(get_generated_files AC_INPUT_FILES)
    find_program(FPP_DEPEND fpp-depend)
    if (DEFINED FPP_TO_DEPEND-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    set(DIRECT_DEPENDENCIES_FILE "${CMAKE_CURRENT_BINARY_DIR}/direct.txt")
    set(INCLUDED_FILE "${CMAKE_CURRENT_BINARY_DIR}/included.txt")
    set(MISSING_FILE "${CMAKE_CURRENT_BINARY_DIR}/missing.txt")
    set(GENERATED_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated.txt")
    set(FRAMEWORK_FILE "${CMAKE_CURRENT_BINARY_DIR}/framework.txt")
    set(LAST_DEP_COMMAND "${FPP_DEPEND} ${FPP_LOCS_FILE} ${AC_INPUT_FILES} -d ${DIRECT_DEPENDENCIES_FILE} -i ${INCLUDED_FILE} -m ${MISSING_FILE} -g ${GENERATED_FILE} -f ${FRAMEWORK_FILE}"
        CACHE INTERNAL "Last command to annotate memo file" FORCE)
    execute_process(COMMAND ${FPP_DEPEND} ${FPP_LOCS_FILE} ${AC_INPUT_FILES}
        -d "${DIRECT_DEPENDENCIES_FILE}"
        -i "${INCLUDED_FILE}"
        -m "${MISSING_FILE}"
        -g "${GENERATED_FILE}"
        -f "${FRAMEWORK_FILE}"
        RESULT_VARIABLE ERR_RETURN
        OUTPUT_VARIABLE STDOUT OUTPUT_STRIP_TRAILING_WHITESPACE)
    # Report failure.  If we are generating files, this must work.
    if (ERR_RETURN)
        message(FATAL_ERROR "Failed to run '${LAST_DEP_COMMAND}' and RC ${ERR_RETURN}")
        return()
    endif()

    # Read files and convert to lists of dependencies. e.g. read INCLUDED_FILE file into INCLUDED variable, then process
    foreach(NAME INCLUDED MISSING GENERATED DIRECT_DEPENDENCIES FRAMEWORK)
        file(READ "${${NAME}_FILE}" "${NAME}")
        string(STRIP "${${NAME}}" "${NAME}")
        string(REGEX REPLACE "\n" ";" "${NAME}" "${${NAME}}")
    endforeach()

    # Handle captured standard out
    string(REGEX REPLACE "\n" ";" IMPORTED "${STDOUT}")
    # List of framework dependencies: detected + builtin, subset from "this module" and further.
    list(APPEND FRAMEWORK ${FPP_FRAMEWORK_DEFAULT_DEPS})
    list(FIND FRAMEWORK "${MODULE_NAME}" START_INDEX)
    math(EXPR START_INDEX "${START_INDEX} + 1")
    list(SUBLIST FRAMEWORK ${START_INDEX} -1 FRAMEWORK)

    # First assemble the generated files list
    set(GENERATED_FILES)
    foreach(LINE IN LISTS GENERATED)
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${LINE}")
    endforeach()
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)

    # If we have missing dependencies, print and fail
    if (MISSING)
        message(WARNING "[autocode/fpp] Missing dependencies ${AC_INPUT_FILES}")
        foreach (MISS IN LISTS MISSING)
            message(WARNING "[autocode/fpp]  ${MISS}")
        endforeach()
        message(FATAL_ERROR)
    endif()

    # Module dependencies are: detected "direct" + framework dependencies
    fpp_to_modules("${DIRECT_DEPENDENCIES}" "${AC_INPUT_FILES}" MODULE_DEPENDENCIES)
    list(APPEND MODULE_DEPENDENCIES ${FRAMEWORK})
    list(REMOVE_DUPLICATES MODULE_DEPENDENCIES)

    # File dependencies are any files that this depends on
    set(FILE_DEPENDENCIES)
    list(APPEND FILE_DEPENDENCIES ${AC_INPUT_FILES} ${INCLUDED})

    # Should have been inherited from previous call to `get_generated_files`
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
    set(EXTRAS "${IMPORTED}" PARENT_SCOPE)
endfunction(get_generated_files)

####
# `get_dependencies`:
#
# Given a set of supported autocoder input files, this will produce a set of dependencies. Since this should have
# already been done in `get_generated_files` the implementation just checks the variables are still set.
#
# - MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
# - FILE_DEPENDENCIES: specific file dependencies of the given input sources
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(get_dependencies AC_INPUT_FILES)
    # Should have been inherited from previous call to `get_generated_files`
    if (NOT DEFINED MODULE_DEPENDENCIES OR NOT DEFINED FILE_DEPENDENCIES)
        message(FATAL_ERROR "The CMake system is inconsistent. Please contact a developer.")
    endif()
endfunction(get_dependencies)

####
# `setup_autocode`:
#
# Sets up the steps to run the autocoder and produce the files during the build. This is passed the lists generated
# in calls to `get_generated_files` and `get_dependencies`.
#
# AC_INPUT_FILES: list of supported autocoder input files
# GENERATED_FILES: a list of files generated for the given input sources
# MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
# FILE_DEPENDENCIES: specific file dependencies of the given input sources
# EXTRAS: used to publish the 'imported' file dependencies of the given input files
####
function(setup_autocode AC_INPUT_FILES GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    find_program(FPP_TO_XML fpp-to-xml)
    find_program(FPP_TO_CPP fpp-to-cpp)
    if (DEFINED FPP_TO_XML-NOTFOUND OR DEFINED FPP_TO_CPP-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    string(REGEX REPLACE ";" ","  FPRIME_BUILD_LOCATIONS_SEP_FPP "${FPRIME_BUILD_LOCATIONS}")
    string(REGEX REPLACE ";" ","  FPP_IMPORTED_SEP "${EXTRAS}")
    set(INCLUDES)
    if (FPP_IMPORTED_SEP)
        set(INCLUDES "-i" "${FPP_IMPORTED_SEP}")
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

    # Add in steps for Ai.xml generation
    get_property(REMOVAL_FILE GLOBAL PROPERTY "FPP_LOCATIONS_CACHE_${MODULE_NAME}")
    if (GENERATED_AI)
        add_custom_command(
                OUTPUT  ${GENERATED_AI}
                COMMAND ${FPP_RUN_OR_REMOVE} ${REMOVAL_FILE} ${FPP_TO_XML} ${AC_INPUT_FILES} "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${INCLUDES}
                    "-p" "${FPRIME_BUILD_LOCATIONS_SEP_FPP}"
                DEPENDS ${AC_INPUT_FILE} ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
    # Add in steps for CPP generation
    if (GENERATED_CPP)
        add_custom_command(
                OUTPUT  ${GENERATED_CPP}
                COMMAND ${FPP_RUN_OR_REMOVE} ${REMOVAL_FILE} ${FPP_TO_CPP} ${AC_INPUT_FILES} "-d" "${CMAKE_CURRENT_BINARY_DIR}" ${INCLUDES}
                "-p" "${FPRIME_BUILD_LOCATIONS_SEP_FPP},${CMAKE_BINARY_DIR}"
                DEPENDS ${AC_INPUT_FILE} ${FILE_DEPENDENCIES} ${MODULE_DEPENDENCIES}
        )
    endif()
endfunction(setup_autocode)

####
# `fpp_to_modules`:
#
# Helper function. Converts a list of files and a list of autocoder inputs into a list of module names.
#
# FILE_LIST: list of files
# AC_INPUT_FILES: list of autocoder input files
# OUTPUT_VAR: output variable to set with result
####
function(fpp_to_modules FILE_LIST AC_INPUT_FILES OUTPUT_VAR)
    init_variables(OUTPUT_DATA)
    get_module_name("${CMAKE_CURRENT_LIST_DIR}")
    set(CURRENT_MODULE "${MODULE_NAME}")
    foreach(INCLUDE IN LISTS AC_INPUT_FILES FILE_LIST)
        get_module_name(${INCLUDE})
        # Here we are adding a module to the modules list if all three of the following are true:
        #  1. Not present already (deduplication)
        #  2. Not the current module directory as learned by the path to the autocoder inputs
        #  3. Not a child of the fprime configuration directory
        # NOTE: item 3 is build on the assumption that configuration .fpp files do not require autocode, but maintain
        # only definitions useful to other modules. This assumption holds as of v3.0.0, but should this assumption break
        # remove the check here, return a known module name (e.g. 'config') for this directory, and place a
        # CMakeLists.txt in that directory that sets up the aforementioned known module and associated target.
        if ("${MODULE_NAME}" IN_LIST OUTPUT_DATA OR CURRENT_MODULE STREQUAL MODULE_NAME OR INCLUDE MATCHES "${FPRIME_CONFIG_DIR}/.*")
            #message(STATUS "Excluding: ${MODULE_NAME} from ${CURRENT_MODULE} with ${INCLUDE}")
            continue() # Skip adding to module list
        endif()
        #message(STATUS "Adding: ${MODULE_NAME} with ${INCLUDE}")
        list(APPEND OUTPUT_DATA "${MODULE_NAME}")
    endforeach()
    set(${OUTPUT_VAR} "${OUTPUT_DATA}" PARENT_SCOPE)
endfunction(fpp_to_modules)
