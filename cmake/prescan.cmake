set(NEEDED_PROPERTIES
    FPRIME_CONFIG_DIR
    FPRIME_AC_CONSTANTS_FILE
    FPRIME_ENVIRONMENT_FILE
    FPRIME_SETTINGS_FILE
    FPRIME_LIBRARY_LOCATIONS
    FPRIME_PROJECT_ROOT
    FPRIME_FRAMEWORK_PATH
    CMAKE_TOOLCHAIN_FILE
    CMAKE_BUILD_TYPE
    CMAKE_DEBUG_OUTPUT
    FPRIME_USE_STUBBED_DRIVERS
    FPRIME_USE_BAREMETAL_SCHEDULER
    FPP_TOOLS_PATH
)
set(PRESCAN_DIR "${CMAKE_BINARY_DIR}/prescan")

function(perform_prescan)
    message(STATUS "Performing CMake source prescan")
    set(CALL_PROPS)
    foreach (PROPERTY IN LISTS NEEDED_PROPERTIES)
        if (NOT "${${PROPERTY}}" STREQUAL "")
            list(APPEND CALL_PROPS "-D${PROPERTY}=${${PROPERTY}}")
        endif()
    endforeach()

    # Run CMake as effeciently as possible
    file(MAKE_DIRECTORY "${PRESCAN_DIR}")
    execute_process(
        COMMAND /usr/bin/time "${CMAKE_COMMAND}"
            -G "${CMAKE_GENERATOR}"
            "${CMAKE_CURRENT_SOURCE_DIR}"
            -DFPRIME_PRESCAN=${CMAKE_BINARY_DIR}
            -DCMAKE_C_COMPILER_FORCED=TRUE
            -DCMAKE_CXX_COMPILER_FORCED=TRUE
            ${CALL_PROPS}
        RESULT_VARIABLE result
        OUTPUT_FILE "${PRESCAN_DIR}/prescan.log"
        WORKING_DIRECTORY "${PRESCAN_DIR}"
        COMMAND_ERROR_IS_FATAL ANY
    )
    file(READ "${CMAKE_BINARY_DIR}/prescan-fpp-list" FPP_LISTING OFFSET 1) # Skips leading ";" preventing null element
    set_property(GLOBAL PROPERTY FP_FPP_LIST ${FPP_LISTING})
    message(STATUS "Performing CMake source prescan -- DONE")
endfunction()