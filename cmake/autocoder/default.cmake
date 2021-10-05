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

###
# `regenerate_memo`:
#
# Default implementation does not regenerate memo upon request of autocoder.
####
function(regenerate_memo OUTPUT MEMO_FILE)
    set(${OUTPUT} FALSE PARENT_SCOPE)
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
