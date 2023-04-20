####
# autocoder/helpers.cmake:
#
# Helper functions to make implementing custom autocoders easier! These codify some of the basic patterns for autocoders
# so that implementers can handle standard functions easily.
####
include_guard()
include("utilities")
set_property(GLOBAL PROPERTY AUTO_RECONFIGURE_LIST)
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
# **REQUIRE_CMAKE_RESCAN:** (optional) this file should trigger a cmake rescan. Default: false
####
macro(autocoder_support_by_suffix SUFFIX AC_INPUT_FILE)
    ends_with(IS_SUPPORTED "${AC_INPUT_FILE}" "${SUFFIX}")
    # Note: set in PARENT_SCOPE in macro is intended. Caller **wants** to set IS_SUPPORTED in their parent's scope.
    set(IS_SUPPORTED "${IS_SUPPORTED}" PARENT_SCOPE)

    # Files that are supported may also be marked as requiring a rescan. This is done through an optional third argument
    if (IS_SUPPORTED AND ${ARGC} GREATER 2)
        # CMake weirdness, if ${ARGC} is <= 2 then ${ARGV2} is inherited not from this macro call, but rather from the
        # calling function.  Thus we need a 2-tier if statement to prevent an explosion.
        # See: https://cmake.org/cmake/help/latest/command/macro.html#argument-caveats
        if (${ARGV2})
            requires_regeneration("${AC_INPUT_FILE}")
        endif()
    endif()
endmacro()

####
# Function `requires_regeneration`:
#
# Called by the autocoder when a source file needs to setup CMake to reconfigure when the source file changes.
#
# `AC_INPUT_FILE`: file to mark as tracked
####
function(requires_regeneration AC_INPUT_FILE)
    get_property(RECONFIGURE_LIST GLOBAL PROPERTY AUTO_RECONFIGURE_LIST)
    get_filename_component(REAL_FILE "${AC_INPUT_FILE}" REALPATH)
    if (NOT "${REAL_FILE}" IN_LIST RECONFIGURE_LIST)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${AC_INPUT_FILE}")
        list(APPEND RECONFIGURE_LIST "${REAL_FILE}")
        set_property(GLOBAL PROPERTY AUTO_RECONFIGURE_LIST "${RECONFIGURE_LIST}")
    endif()
endfunction()

####
# Function `_set_autocoder_name`:
#
# Function to set AUTOCODER_NAME in parent scope. Helper to the below two function, not intended for users to call this
# function.
#
# FUNCTION_NAME: function name for producing error message
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
