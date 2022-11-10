set(SETTINGS_CMAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
find_program(PYTHON NAMES python3 python) #This happens before required

function(ini_to_cache)
    set(CALCULATED_INI "${CMAKE_SOURCE_DIR}/settings.ini")

    # Check if settings.ini is defined and is not what is expected
    if (DEFINED FPRIME_SETTINGS_FILE AND NOT FPRIME_SETTINGS_FILE STREQUAL "${CALCULATED_INI}")
        message(FATAL_ERROR "Expected settings.ini not equal to calculated. '${FPRIME_SETTINGS_FILE}' != '${CALCULATED_INI}'")
    endif()
    # Execute the process
    execute_process(COMMAND ${PYTHON}
        "${SETTINGS_CMAKE_DIRECTORY}/ini-to-stdio.py"
        "${CALCULATED_INI}"
        ${CMAKE_TOOLCHAIN_FILE}
        OUTPUT_VARIABLE INI_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Turn output into a list of lines, escaping possible ;
    STRING(REGEX REPLACE ";" "\\\\;" INI_OUTPUT "${INI_OUTPUT}")
    STRING(REGEX REPLACE "\n" ";" INI_OUTPUT "${INI_OUTPUT}")

    # Process line-by-line
    foreach(LINE IN LISTS INI_OUTPUT)
        STRING(REGEX REPLACE "\\|" ";" LINE "${LINE}")
        list(GET LINE 0 SETTING)
        list(LENGTH LINE ELEMENTS)
        if (ELEMENTS GREATER 1)
            list(GET LINE 1 VALUE)
        else()
            set(VALUE "")
        endif()
        string(TOUPPER "${SETTING}" SETTING)
        STRING(REGEX REPLACE ":" ";" VALUE "${VALUE}")

        set(SETTING_VARIABLE_NAME "FPRIME_${SETTING}")

        # If not defined, load the setting into the cache. Otherwise check for discrepancies.
        if (NOT DEFINED ${SETTING_VARIABLE_NAME})
            set(${SETTING_VARIABLE_NAME} "${VALUE}" CACHE INTERNAL "")
        elseif(NOT "${VALUE}" STREQUAL "${${SETTING_VARIABLE_NAME}}")
            message(FATAL_ERROR "settings.ini values changed. Please regenerate cache. ${SETTING_VARIABLE_NAME} changed from |${${SETTING_VARIABLE_NAME}}| to |${VALUE}|")
        endif()
    endforeach()
endfunction(ini_to_cache)
