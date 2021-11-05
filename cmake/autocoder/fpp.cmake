include(utilities)
include(autocoder/default)


set(FPP_RUN_OR_REMOVE "${CMAKE_CURRENT_LIST_DIR}/fpp-wrapper/fpp-run-or-remove")

set(HANDLES_INDIVIDUAL_SOURCES FALSE)

function(is_supported AC_INPUT_FILE)
    if (AC_INPUT_FILE MATCHES ".*.fpp")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    else()
        set(IS_SUPPORTED FALSE PARENT_SCOPE)
    endif()
endfunction(is_supported)

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

function(get_generated_files AC_INPUT_FILES)
    find_program(FPP_DEPEND fpp-depend)
    if (DEFINED FPP_TO_DEPEND-NOTFOUND)
        message(FATAL_ERROR "fpp tools not found, please install them onto your system path")
    endif()
    set(DIRECT_FILE "${CMAKE_CURRENT_BINARY_DIR}/direct.txt")
    set(INCLUDED_FILE "${CMAKE_CURRENT_BINARY_DIR}/included.txt")
    set(MISSING_FILE "${CMAKE_CURRENT_BINARY_DIR}/missing.txt")
    set(GENERATED_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated.txt")
    set(LAST_DEP_COMMAND "${FPP_DEPEND} ${FPP_LOCS_FILE} ${AC_INPUT_FILES} -d ${DIRECT_FILE} -i ${INCLUDED_FILE} -m ${MISSING_FILE} -g ${GENERATED_FILE}"
        CACHE INTERNAL "Last command to annotate memo file" FORCE)
    execute_process(COMMAND ${FPP_DEPEND} ${FPP_LOCS_FILE} ${AC_INPUT_FILES}
        -d "${DIRECT_FILE}"
        -i "${INCLUDED_FILE}"
        -m "${MISSING_FILE}"
        -g "${GENERATED_FILE}"
        RESULT_VARIABLE ERR_RETURN
        OUTPUT_VARIABLE STDOUT OUTPUT_STRIP_TRAILING_WHITESPACE)
    # Report failure.  If we are generating files, this must work.
    if (ERR_RETURN)
        message(FATAL_ERROR "Failed to run '${LAST_DEP_COMMAND}' and RC ${ERR_RETURN}")
        return()
    endif()

    # Read file and convert to lists of dependencies
    file(READ "${INCLUDED_FILE}" INCLUDED)
    file(READ "${MISSING_FILE}" MISSING)
    file(READ "${GENERATED_FILE}" GENERATED)
    file(READ "${DIRECT_FILE}" DIRECT_DEPENDENCIES)

    string(STRIP "${INCLUDED}" INCLUDED)
    string(STRIP "${MISSING}" MISSING)
    string(STRIP "${DIRECT_DEPENDENCIES}" DIRECT_DEPENDENCIES)
    string(STRIP "${GENERATED}" GENERATED)

    string(REGEX REPLACE "\n" ";" IMPORTED "${STDOUT}")
    string(REGEX REPLACE "\n" ";" INCLUDED "${INCLUDED}")
    string(REGEX REPLACE "\n" ";" MISSING "${MISSING}")
    string(REGEX REPLACE "\n" ";" DIRECT_DEPENDENCIES "${DIRECT_DEPENDENCIES}")
    string(REGEX REPLACE "\n" ";" GENERATED "${GENERATED}")

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
    fpp_to_modules("${DIRECT_DEPENDENCIES}" "${AC_INPUT_FILES}" MODULE_DEPENDENCIES)
    set(FILE_DEPENDENCIES)
    list(APPEND FILE_DEPENDENCIES ${AC_INPUT_FILES} ${INCLUDED})
    # TODO: fix-me
    if (NOT "${MODULE_NAME}" STREQUAL "Os" AND NOT "${MODULE_NAME}" MATCHES "^Fw_")
        foreach(KNOWN IN ITEMS "Fw_Cfg" "Fw_Types" "Fw_Time" "Fw_Com" "Os" "Fw_Tlm" "Fw_Cmd" "Fw_Log" "Fw_Prm" "Fw_Comp" "Fw_CompQueued")
            list(APPEND MODULE_DEPENDENCIES "${KNOWN}")
        endforeach()
    endif()
    # Should have been inherited from previous call to `get_generated_files`
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
    set(EXTRAS "${IMPORTED}" PARENT_SCOPE)
endfunction(get_generated_files)

function(get_dependencies AC_INPUT_FILES)
    # Should have been inherited from previous call to `get_generated_files`
    if (NOT DEFINED MODULE_DEPENDENCIES OR NOT DEFINED FILE_DEPENDENCIES)
        message(FATAL "The CMake system is inconsistent. Please contact a developer.")
    endif()
endfunction(get_dependencies)

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

function(fpp_to_modules FILE_LIST AC_INPUT_FILES OUTPUT_VAR)
    init_variables(OUTPUT AI_DIR)
    foreach(AC_INPUT IN LISTS AC_INPUT_FILES)
        get_filename_component(AC_INPUT_FILE "${AC_INPUT}" DIRECTORY)
        build_relative_path("${AC_INPUT_FILE}" AI_DIR_TEMP)
        # Check for set but not equal
        if (AI_DIR AND NOT AI_DIR STREQUAL AI_DIR_TEMP)
            message(FATAL_ERROR "Autocoder inputs span multiple directories: ${AI_DIR} and ${AI_DIR_TEMP}")
        endif()
        set(AI_DIR "${AI_DIR_TEMP}")
    endforeach()
    foreach(INCLUDE IN LISTS AC_INPUT_FILES FILE_LIST)
        get_filename_component(MODULE_DIR "${INCLUDE}" DIRECTORY)
        build_relative_path("${MODULE_DIR}" MODULE_DIR)
        string(REPLACE "/" "_" MODULE_NAME "${MODULE_DIR}")
        if (NOT "${MODULE_NAME}" IN_LIST OUTPUT AND NOT AI_DIR STREQUAL MODULE_DIR)
            list(APPEND OUTPUT "${MODULE_NAME}")
        endif()
    endforeach()
    set(${OUTPUT_VAR} "${OUTPUT}" PARENT_SCOPE)
endfunction(fpp_to_modules)
