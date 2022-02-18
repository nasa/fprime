####
# autocoder/helpers.cmake:
#
# Helper functions to make implementing custom autocoders easier! These codify some of the basic patterns for autocoders
# so that implementers can handle standard functions easily.
####
include_guard()
####
# Macro `autocoder_support_by_suffix`:
#
# This sets up an autocoder to handle files based on a suffix. For example, passing in "*.fpp" will support all files
# ending in ".fpp" i.e. FPP files or passing in "ComponentAi.xml" will support all component XMLs. It is implemented as
# a macro such that users need not do anything other than call it with the suffix to setup the system correctly. This
# performs raw ascii comparison, not regular expression matching.
#
# **Note:** this will set the appropriate variable in PARENT_SCOPE and since it is macro this will be the parent scope
# of the caller.
#
# **SUFFIX**: suffix to support
# **AC_INPUT_FILE**: file to check with suffix
####
macro(autocoder_support_by_suffix SUFFIX AC_INPUT_FILE)
    set(IS_SUPPORTED FALSE PARENT_SCOPE)
    # Suffix is the length
    string(LENGTH "${AC_INPUT_FILE}" INPUT_LENGTH)
    string(LENGTH "${SUFFIX}" SUFFIX_LENGTH)
    if (INPUT_LENGTH GREATER_EQUAL SUFFIX_LENGTH)
        # Calculate the substring of suffix length at end of string
        math(EXPR START "${INPUT_LENGTH} - ${SUFFIX_LENGTH}")
        string(SUBSTRING "${AC_INPUT_FILE}" "${START}" "${SUFFIX_LENGTH}" FOUND_SUFFIX)
        # Check the substring
        if (FOUND_SUFFIX STREQUAL "${SUFFIX}")
            set(IS_SUPPORTED TRUE PARENT_SCOPE)
        endif()
    endif()
endmacro()

####
# Function `_set_autocoder_name`:
#
# Function to set AUTOCODER_NAME in parent scope. Helper to the below two function, not intended for users to call this
# function.
#
# FUNCTION_NAME: function name for producing error mesage
####
function(_set_autocoder_name FUNCTION_NAME)
    get_filename_component(AUTOCODER_DIRPATH "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
    get_filename_component(AUTOCODER_DIRNAME "${AUTOCODER_DIRPATH}" NAME)
    get_filename_component(AUTOCODER_NAME "${CMAKE_CURRENT_LIST_FILE}" NAME_WLE)

    # Some basic check
    if (AUTOCODE_NAME STREQUAL "CMakeList")
        message(FATAL_ERROR "${FUNCTION_NAME} may only be called globally and from within an autocoder CMake file")
    # ASSERT we are not getting the name from autocoder/autocoder.cmake
    elseif(AUTOCODER_DIRNAME STREQUAL "autocoder" AND AUTOCODER_NAME STREQUAL "autocoder")
        message(FATAL_ERROR "CMake code is inconsistent")
    # ASSERT we are not getting the name from autocoder/helpers.cmake
    elseif(AUTOCODER_DIRNAME STREQUAL "autocoder" AND AUTOCODER_NAME STREQUAL "helpers")
        message(FATAL_ERROR "CMake code is inconsistent")
    # ASSERT we are not getting the name from utilities.cmake
    elseif(AUTOCODER_DIRNAME STREQUAL "cmake" AND AUTOCODER_NAME STREQUAL "utilities")
        message(FATAL_ERROR "CMake code is inconsistent")
    endif()
    set(AUTOCODER_NAME "${AUTOCODER_NAME}" PARENT_SCOPE)
endfunction()

####
# Macro `autocoder_setup_for_individual_sources`:
#
# A helper to setup the autocoder to handle individual sources. Each source input to the autocoder will result in a new
# call through the autocoder. This handles setting the appropriate property such that this autocoder need not know nor
# care about the property itself. This may only be called from within an autocoder file.
###
function(autocoder_setup_for_individual_sources)
    _set_autocoder_name(autocoder_setup_for_individual_sources)
    set_property(GLOBAL PROPERTY "${AUTOCODER_NAME}_HANDLES_INDIVIDUAL_SOURCES" TRUE)
endfunction()

####
# Macro `autocoder_setup_for_multiple_sources`:
#
# A helper to setup the autocoder to handle multiple sources with one invocation. All supported source for the module
# will be supplied to a single invocation of the autocoder. This handles setting the appropriate property such that this
# autocoder need not know nor care about the property itself. This may only be called from within an autocoder file.
###
function(autocoder_setup_for_multiple_sources)
    _set_autocoder_name(autocoder_setup_for_multiple_sources)
    set_property(GLOBAL PROPERTY "${AUTOCODER_NAME}_HANDLES_INDIVIDUAL_SOURCES" FALSE)
endfunction()