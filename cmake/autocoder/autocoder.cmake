include(Utilities)

set(AUTOCODER_LIST CACHE INTERNAL "List of known autocoders")



function(register_fprime_autocoder AUTOCODER_CMAKE)
    # Check existence of autocoder file
    if (NOT EXISTS "${AUTOCODER_CMAKE}")
        message(FATAL_ERROR "[Autocode] ${AUTOCODER_CMAKE} does not exist")
    endif()
    get_filename_component(AUTOCODER_NAME "${AUTOCODER_CMAKE}" NAME_WE)
    include("${AUTOCODER_CMAKE}")
    # Validation steps for the included autocoder
    foreach(EXPECTED_COMMAND "is_supported" "get_generated_files" "get_dependencies" "setup_autocode")
        if (NOT COMMAND "${EXPECTED_COMMAND}")
            # TODO: clean-up.  Make this a warning.  FATAL to ensure that I know when I have failed.
            message(FATAL_ERROR "[Autocode] '${AUTOCODER_NAME}' autocoder must define '${EXPECTED_COMMAND}'")
        endif()
    endforeach()
    list(APPEND AUTOCODER_LIST "${AUTOCODER_CMAKE}")
    set(AUTOCODER_LIST "${AUTOCODER_LIST}" CACHE INTERNAL "List of known autocoders")
    message(STATUS "[Autocode] Registered '${AUTOCODER_NAME}' autocoder")
endfunction(register_fprime_autocoder AUTOCODER_CMAKE)


function (ac_process_sources SOURCES)

    set(MODULE_DEPENDENCIES_LIST)
    set(GENERATED_FILE_LIST)
    set(CONSUMED_SOURCES_LIST)
    foreach(AC_CMAKE IN LISTS AUTOCODER_LIST)
        # Clear return variables
        set(MODULE_DEPENDENCIES)
        set(GENERATED_FILES)
        set(CONSUMED_SOURCES)
        __ac_process_sources("${AC_CMAKE}" "${SOURCES}" "${GENERATED_FILE_LIST}")
        list(APPEND MODULE_DEPENDENCIES_LIST ${MODULE_DEPENDENCIES})
        list(APPEND GENERATED_FILE_LIST ${GENERATED_FILES})
        list(APPEND CONSUMED_SOURCES_LIST ${CONSUMED_SOURCES})
    endforeach()

    # Return variables
    set(AC_DEPENDENCIES "${MODULE_DEPENDENCIES_LIST}" PARENT_SCOPE)
    set(AC_GENERATED "${GENERATED_FILE_LIST}" PARENT_SCOPE)
    set(AC_SOURCES "${CONSUMED_SOURCES_LIST}" PARENT_SCOPE)
endfunction()

####
# __memoize:
#
# This take two "long" processing steps of the autocoder that run during the configuration step of CMake and notes the
# output such that the results are cached and repeated unless the input file has changed since the last pass through
# this function. This is done for efficiency when the generate dependencies or generate files step takes a large
# execution costs.
#
# Note: cached variables are the following: GENERATED_FILES, MODULE_DEPENDENCIES, FILE_DEPENDENCIES
#
# SOURCE: source file that is being parsed and must have changed for a recalculation
####
function (__memoize SOURCE)
    get_filename_component(SOURCE_NAME "${SOURCE}" NAME)
    set(MEMO_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE_NAME}.dep")

    # Run the expensive action only if the input has changed
    on_changed("${SOURCE}" CHANGED)
    if (CHANGED OR NOT EXISTS "${MEMO_FILE}")
        get_generated_files("${SOURCE}")
        get_dependencies("${SOURCE}")
        file(WRITE "${MEMO_FILE}" "${GENERATED_FILES}\n${MODULE_DEPENDENCIES}\n${FILE_DEPENDENCIES}\n${EXTRAS}\n")
    # Otherwise read from file
    else()
        file(READ "${MEMO_FILE}" CONTENTS)
        read_from_lines("${CONTENTS}" GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    endif()
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
    set(EXTRAS "${EXTRAS}" PARENT_SCOPE)
endfunction()

function(__ac_process_sources AUTOCODER_CMAKE SOURCES GENERATED_SOURCES)
    set(MODULE_DEPENDENCIES_LIST)
    set(GENERATED_FILE_LIST)
    set(CONSUMED_SOURCES_LIST)

    get_filename_component(AUTOCODER_NAME "${AUTOCODER_CMAKE}" NAME_WE)
    include("${AUTOCODER_CMAKE}")
    foreach(SOURCE IN LISTS SOURCES GENERATED_SOURCES)
        # Clear return variables
        set(MODULE_DEPENDENCIES)
        set(GENERATED_FILES)

        # Convert the input file into a real path to ensure the system knows what to work with
        get_filename_component(SOURCE "${SOURCE}" REALPATH)
        __ac_process_source("${SOURCE}")

        # Check if this would have generated something, if not don't mark the file as used
        if (GENERATED_FILES)
            list(APPEND CONSUMED_SOURCES_LIST "${SOURCE}")
        endif()

        list(APPEND MODULE_DEPENDENCIES_LIST ${MODULE_DEPENDENCIES})
        list(APPEND GENERATED_FILE_LIST ${GENERATED_FILES})
    endforeach()

    # Return variables
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES_LIST}" PARENT_SCOPE)
    set(GENERATED_FILES "${GENERATED_FILE_LIST}" PARENT_SCOPE)
    set(CONSUMED_SOURCES "${CONSUMED_SOURCES_LIST}" PARENT_SCOPE)
endfunction()


function(__ac_process_source SOURCE)
    # Check support of this file type
    is_supported("${SOURCE}")
    # Bail when not supported
    if (NOT IS_SUPPORTED)
        return()
    endif()
    # Run the autocode setup process now with memoization
    __memoize("${SOURCE}")

    # Explain everything
    if (CMAKE_DEBUG_OUTPUT)
        message(STATUS "[Autocode/${AUTOCODER_NAME}] Autocoding '${SOURCE}'")
        message(STATUS "[Autocode/${AUTOCODER_NAME}] Generated Files:")
        foreach(GENERATED_FILE IN LISTS GENERATED_FILES)
            message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${GENERATED_FILE}")
        endforeach()
        # Output file dependency status block
        if (FILE_DEPENDENCIES)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] File Dependencies:")
            foreach(FILE_DEPENDENCY IN LISTS FILE_DEPENDENCIES)
                message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${FILE_DEPENDENCY}")
            endforeach()
        endif()
        # Output module dependency status block
        if (MODULE_DEPENDENCIES)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] Module Dependencies:")
            foreach(MODULE_DEPENDENCY IN LISTS MODULE_DEPENDENCIES)
                message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${MODULE_DEPENDENCY}")
            endforeach()
        endif()
    endif()
    setup_autocode("${SOURCE}" "${GENERATED_FILES}" "${MODULE_DEPENDENCIES}" "${FILE_DEPENDENCIES}" "${EXTRAS}")

    # Return values
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)

    # TODO: set dependent with-in cmake
endfunction()