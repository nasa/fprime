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
# init_to_cache:
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
    )

    # Process line-by-line
    foreach(LINE IN LISTS INI_OUTPUT)
        STRING(REPLACE ";" "\\;" LINE "${LINE}")
        STRING(REPLACE "=" ";" LINE "${LINE}")
        list(GET LINE 0 SETTING)
        list(LENGTH LINE ELEMENTS)
        if (ELEMENTS GREATER 1)
            list(GET LINE 1 VALUE)
        else()
            set(VALUE "")
        endif()

        # If not defined, load the setting into the cache.
        if (NOT DEFINED ${SETTING})
            # Print value when debugging
            if (CMAKE_DEBUG_OUTPUT)
                message(STATUS "${SETTING} read from settings.ini as '${VALUE}'")
            endif()
            set(${SETTING}_ORIGINAL "${VALUE}" CACHE INTERNAL "Original value of ${SETTING} from settings.ini")
            set(${SETTING} "${VALUE}" CACHE INTERNAL "")
        # If setting was originally loaded, here, from settings.ini, then check it did not change
        elseif(DEFINED ${SETTING}_ORIGINAL AND NOT "${VALUE}" STREQUAL "${${SETTING}_ORIGINAL}")
            # Print some extra output to help debug
            if (CMAKE_DEBUG_OUTPUT)
                message(WARNING "${SETTING} changed from '${${SETTING}_ORIGINAL}' to '${VALUE}'")
            endif()
            message(FATAL_ERROR "settings.ini field changed. Please regenerate.")
        endif()
    endforeach()
endfunction(ini_to_cache)
