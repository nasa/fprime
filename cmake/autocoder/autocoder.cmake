include(utilities)

function (run_ac_set SOURCES)
    # Get the source list, if passed in
    set(AC_LIST)
    if (ARGN)
        set(AC_LIST "${ARGN}")
    endif()
    set(INFO_ONLY OFF)
    init_variables(MODULE_DEPENDENCIES_LIST GENERATED_FILE_LIST CONSUMED_SOURCES_LIST)
    foreach(AC_CMAKE IN LISTS AC_LIST)
        init_variables(MODULE_DEPENDENCIES GENERATED_FILES CONSUMED_SOURCES)
        # Allow info only runs
        if (AC_CMAKE STREQUAL "INFO_ONLY")
            set(INFO_ONLY ON)
            continue()
        endif()
        run_ac("${AC_CMAKE}" "${SOURCES}" "${GENERATED_FILE_LIST}" "${INFO_ONLY}")
        list(APPEND MODULE_DEPENDENCIES_LIST ${MODULE_DEPENDENCIES})
        list(APPEND GENERATED_FILE_LIST ${GENERATED_FILES})
        list(APPEND CONSUMED_SOURCES_LIST ${CONSUMED_SOURCES})
        set(INFO_ONLY OFF)
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
# SOURCES: source file that is being parsed and must have changed for a recalculation
####
function (__memoize SOURCES)
    string(MD5 SOURCES_HASH "${SOURCES}")
    set(MEMO_FILE "${CMAKE_CURRENT_BINARY_DIR}/${AUTOCODER_NAME}.${SOURCES_HASH}.dep")

    # Run the expensive action only if the input has changed
    on_any_changed("${SOURCES}" CHANGED)
    if(COMMAND regenerate_memo)
        regenerate_memo(FORCE_REGENERATE "${MEMO_FILE}")
    endif()
    if (CHANGED OR FORCE_REGENERATE OR NOT EXISTS "${MEMO_FILE}")
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] Regenerating memo '${MEMO_FILE}' because: (changed: ${CHANGED}, forced: ${FORCE_REGENERATE})")
        endif()
        get_generated_files("${SOURCES}")
        get_dependencies("${SOURCES}")
        resolve_dependencies(MODULE_DEPENDENCIES ${MODULE_DEPENDENCIES})
        file(WRITE "${MEMO_FILE}" "${GENERATED_FILES}\n${MODULE_DEPENDENCIES}\n${FILE_DEPENDENCIES}\n${EXTRAS}\n${LAST_DEP_COMMAND}\n")
    # Otherwise read from file
    else()
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] Using memo ${MEMO_FILE}'")
        endif()
        file(READ "${MEMO_FILE}" CONTENTS)
        read_from_lines("${CONTENTS}" GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    endif()
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
    set(EXTRAS "${EXTRAS}" PARENT_SCOPE)
endfunction()

function(run_ac AUTOCODER_CMAKE SOURCES GENERATED_SOURCES INFO_ONLY)
    include(autocoder/default) # Default function definitions perform validation
    include(${AUTOCODER_CMAKE})
    set(AUTOCODER_NAME "${AUTOCODER_CMAKE}")

    normalize_paths(AC_INPUT_SOURCES "${SOURCES}" "${GENERATED_SOURCES}")
    _filter_sources(AC_INPUT_SOURCES "${AC_INPUT_SOURCES}")
    if (NOT AC_INPUT_SOURCES)
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] No sources detected")
        endif()
        return()
    endif()

    # Start by displaying inputs to autocoders
    if (CMAKE_DEBUG_OUTPUT AND NOT INFO_ONLY)
        message(STATUS "[Autocode/${AUTOCODER_NAME}] Autocoding Input Sources:")
        foreach(SOURCE IN LISTS AC_INPUT_SOURCES)
            message(STATUS "[Autocode/${AUTOCODER_NAME}]   ${SOURCE}")
        endforeach()
    endif()

    set(CONSUMED_SOURCES)
    # Handles individual/multiple source handling
    if (HANDLES_INDIVIDUAL_SOURCES)
        init_variables(MODULE_DEPENDENCIES_LIST GENERATED_FILES_LIST)
        foreach(SOURCE IN LISTS AC_INPUT_SOURCES)
            __ac_process_sources("${SOURCE}" "${INFO_ONLY}")
            list(APPEND MODULE_DEPENDENCIES_LIST ${MODULE_DEPENDENCIES})
            list(APPEND GENERATED_FILES_LIST ${GENERATED_FILES})
            # Check if this would have generated something, if not don't mark the file as used
            if (GENERATED_FILES)
                list(APPEND CONSUMED_SOURCES "${SOURCE}")
            endif()
        endforeach()
        set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES_LIST}")
        set(GENERATED_FILES "${GENERATED_FILES_LIST}")
    else()
        __ac_process_sources("${AC_INPUT_SOURCES}" "${INFO_ONLY}")
        if (GENERATED_FILES)
            set(CONSUMED_SOURCES "${AC_INPUT_SOURCES}")
        endif()
    endif()
    # When actually generating items, explain what is done and why
    if (CMAKE_DEBUG_OUTPUT AND NOT INFO_ONLY)
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
    # Configure depends on this source file if it causes a change to module dependencies
    if (MODULE_DEPENDENCIES)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SOURCES})
    endif()

    # Return variables
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
    set(CONSUMED_SOURCES "${CONSUMED_SOURCES}" PARENT_SCOPE)
endfunction(run_ac)

####
# _filter_sources:
#
# Filters sources down to the ones supported by the active autocoder. It is an error to call this helper function before
# including an autocoder's CMake file and thus setting the active autocoder.
#
# OUTPUT_NAME: name of output variable to set in parent scope
# ...: any number of arguments containing lists of sources
####
function(_filter_sources OUTPUT_NAME)
    set(OUTPUT_LIST)
    # Loop over the list and check
    foreach (SOURCE_LIST IN LISTS ARGN)
        foreach(SOURCE IN LISTS SOURCE_LIST)
            is_supported("${SOURCE}")
            if (IS_SUPPORTED)
                list(APPEND OUTPUT_LIST "${SOURCE}")
            endif()
        endforeach()
    endforeach()
    set(${OUTPUT_NAME} "${OUTPUT_LIST}" PARENT_SCOPE)
endfunction(_filter_sources)


function(__ac_process_sources SOURCES INFO_ONLY)
    # Run the autocode setup process now with memoization
    __memoize("${SOURCES}")
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)

    # Run the generation setup when not requesting "info only"
    if (NOT INFO_ONLY)
        setup_autocode("${SOURCES}" "${GENERATED_FILES}" "${MODULE_DEPENDENCIES}" "${FILE_DEPENDENCIES}" "${EXTRAS}")
    endif()
endfunction()

