**Note:** auto-generated from comments in: ./autocoder/default.cmake

## autocoder/default.cmake:

Default definitions to autocoder functions ensuring that no cross-autocoder setup occurs.


## `is_supported`:

Default implementation is an error.  This must be implemented.


## `default_regenerate_helper`:

Default regenerate helper, so that other regenerates can be built on top of this.


## function(regenerate_memo OUTPUT MEMO_FILE SOURCES_INPUT)
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



## function(get_generated_files AC_INPUT_FILES)
    message(FATAL_ERROR "${AUTOCODER_NAME} must define 'get_generated_files'")
endfunction(get_generated_files)



## function(get_dependencies AC_INPUT_FILES)
    message(FATAL_ERROR "${AUTOCODER_NAME} must define 'get_dependencies'")
endfunction(get_dependencies)



## function(setup_autocode AC_INPUT_FILES GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    message(FATAL_ERROR "${AUTOCODER_NAME} must define 'setup_autocode'")
endfunction(setup_autocode)
