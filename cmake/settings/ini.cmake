####
# ini.cmake:
#
# This file loads settings from INI files. In cases where these settings are supplied via fprime-util, the settings are
# checked for discrepancy. This allows the cmake system to detect when it should be regenerated and has not been. In the
# case that the settings were not supplied, it sets them.
####
include_guard()

# Necessary global variables
set(SETTINGS_CMAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
find_program(PYTHON NAMES python3 python) #This happens before required

####
# FPRIME_UTIL_CRITICAL_LIST:
#
# This is a list of critical settings that are passed in from fprime-util. If these settings change in `settings.ini`
# they need to result in a WARNING to let the user know that had fprime-util be run, it now needs to be rerun.
####
set(FPRIME_UTIL_CRITICAL_LIST
    "FPRIME_FRAMEWORK_PATH"
    "FPRIME_LIBRARY_LOCATIONS"
    "FPRIME_PROJECT_ROOT"
    "FPRIME_ENVIRONMENT_FILE"
    "FPRIME_AC_CONSTANTS_FILE"
    "FPRIME_CONFIG_DIR"
    "FPRIME_INSTALL_DEST"
)

####
# ini_to_cache:
#
# Uses a python script to load INI files. These items are set into the CMake cache.
####
function(ini_to_cache)
    set(CALCULATED_INI "${CMAKE_SOURCE_DIR}/settings.ini")

    # Check if settings.ini is defined and is not what is expected
    if (DEFINED FPRIME_SETTINGS_FILE AND NOT FPRIME_SETTINGS_FILE STREQUAL "${CALCULATED_INI}")
        message(FATAL_ERROR "Provided settings.ini '${FPRIME_SETTINGS_FILE}' not expected file '${CALCULATED_INI}'")
    endif()
    # Execute the process
    execute_process(COMMAND ${PYTHON}
        "${SETTINGS_CMAKE_DIRECTORY}/ini-to-stdio.py"
        "${CALCULATED_INI}"
        ${CMAKE_TOOLCHAIN_FILE}
        OUTPUT_VARIABLE INI_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE RESULT_CODE
    )
    # Check result code
    if (NOT RESULT_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to process settings.ini file: ${CALCULATED_INI}")
    endif()

    # Unset the CMAKE_INSTALL_PREFIX as we override it
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR CMAKE_INSTALL_PREFIX STREQUAL "")
        unset(CMAKE_INSTALL_PREFIX CACHE)
    endif()
    # Process line-by-line
    foreach(LINE IN LISTS INI_OUTPUT)
        # Skip malformed lines
        if (NOT LINE MATCHES "^[A-Za-z0-9_]+=")
            message(STATUS "${LINE}")
            continue()
        endif()
        STRING(REPLACE ";" "\\;" LINE "${LINE}")
        STRING(REPLACE "=" ";" LINE "${LINE}")
        list(GET LINE 0 SETTING)
        list(LENGTH LINE ELEMENTS)
        if (ELEMENTS GREATER 1)
            list(GET LINE 1 VALUE)
        else()
            set(VALUE "")
        endif()

        # Here we set several cache variables:
        # - <setting>_INI_: original setting, but was loaded from settings.ini
        # - <setting>_CLI_: original setting, but was passed in via CLI
        # These are used to detect changes and alert the user.
        # If the setting is undefined, then we must load it from the INI file and set the proper value.
        if (NOT DEFINED "${SETTING}")
            # Print source of setting when debugging
            if (CMAKE_DEBUG_OUTPUT)
                message(STATUS "${SETTING} read from settings.ini as '${VALUE}'")
            endif()
            set("${SETTING}_INI_" "${VALUE}" CACHE INTERNAL "Original value of ${SETTING} from settings.ini")
            set("${SETTING}" "${VALUE}" CACHE INTERNAL "")
        # If setting was originally loaded, here, from settings.ini. We should check that it is correctly re-set.
        elseif(DEFINED "${SETTING}_INI_")
            # Changed INI files are hard-failure as it is difficult to know how/when to regenerate
            if(NOT "${VALUE}" STREQUAL "${${SETTING}_INI_}")
                # Print some extra output to help debug
                if (CMAKE_DEBUG_OUTPUT)
                    message(WARNING "${SETTING} changed from '${${SETTING}_INI_}' to '${VALUE}'")
                endif()
                message(FATAL_ERROR "settings.ini field changed. Please regenerate.")
            endif()
        # If setting was passed in on CLI
        elseif(DEFINED "${SETTING}_CLI_")
            # Changed INI files are hard-failure as it is difficult to know how/when to regenerate
            if(NOT "${VALUE}" STREQUAL "${${SETTING}_CLI_}" AND SETTING IN_LIST FPRIME_UTIL_CRITICAL_LIST)
                # Print some extra output to help debug
                if (CMAKE_DEBUG_OUTPUT)
                    message(WARNING "${SETTING} changed from '${${SETTING}_CLI_}' to '${VALUE}'")
                endif()
                message(WARNING "settings.ini field changed. This likely means fprime-util generate should be run.")
            endif()
        # Setting defined, but none of the check-values are set. This it is the first run, with items from CLI.
        else()
            # Print source of setting when debugging
            if (CMAKE_DEBUG_OUTPUT)
                message(STATUS "${SETTING} read from CLI as '${${SETTING}}'")
            endif()
            set("${SETTING}_CLI_" "${${SETTING}}" CACHE INTERNAL "Original value of ${SETTING} from CLI")
        endif()
    endforeach()
endfunction(ini_to_cache)
