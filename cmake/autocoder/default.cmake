####
# autocoder/default.cmake:
#
# Default definitions to autocoder functions ensuring that no cross-autocoder setup occurs.
####

set(HANDLES_INDIVIDUAL_SOURCES TRUE)
####
# `is_supported`:
#
# Default implementation is an error.  This must be implemented.
####
function(is_supported AC_INPUT_FILE)
    message(FATAL_ERROR "${AUTOCODER_CMAKE} must define 'is_supported'")
endfunction(is_supported)


####
# `default_regenerate_helper`:
#
# Default regenerate helper, so that other regenerates can be built on top of this.
####
function(default_regenerate_helper MISSING_VAR CHANGED_VAR MEMO_FILE SOURCES_INPUT)
    # Check for missing memo file
    set(MEMO_MISSING TRUE)
    if (EXISTS "${MEMO_FILE}")
        set(MEMO_MISSING FALSE)
    endif()
    # Changed sources
    on_any_changed("${SOURCES_INPUT}" CHANGED)
    set(${MISSING_VAR} ${MEMO_MISSING} PARENT_SCOPE)
    set(${CHANGED_VAR} ${CHANGED} PARENT_SCOPE)
endfunction(default_regenerate_helper)


###
# `regenerate_memo`:
#
# Default implementation does not regenerate memo upon request of autocoder.
####
function(regenerate_memo OUTPUT MEMO_FILE SOURCES_INPUT)
    set(${OUTPUT} FALSE PARENT_SCOPE)
    default_regenerate_helper(MEMO_MISSING CHANGED "${MEMO_FILE}" "${SOURCES_INPUT}")
    # Regenerating on any of the above
    if (MEMO_MISSING OR CHANGED)
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[Autocode/${AUTOCODER_NAME}] Regenerating memo '${MEMO_FILE}' because: (memo missing: ${MEMO_MISSING}, sources changed: ${CHANGED})")
        endif()
	set(${OUTPUT} TRUE PARENT_SCOPE)
    endif()
endfunction(regenerate_memo)

####
# `get_generated_files`:
#
# Default implementation is an error.  This must be implemented.
####
function(get_generated_files AC_INPUT_FILES)
    message(FATAL_ERROR "${AUTOCODER_NAME} must define 'get_generated_files'")
endfunction(get_generated_files)

####
# `get_dependencies`:
#
# Default implementation is an error.  This must be implemented.
####
function(get_dependencies AC_INPUT_FILES)
    message(FATAL_ERROR "${AUTOCODER_NAME} must define 'get_dependencies'")
endfunction(get_dependencies)

####
# `setup_autocode`:
#
# Default implementation is an error.  This must be implemented.
####
function(setup_autocode AC_INPUT_FILES GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    message(FATAL_ERROR "${AUTOCODER_NAME} must define 'setup_autocode'")
endfunction(setup_autocode)
