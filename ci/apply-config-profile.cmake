# CI-only bridge from config-profiles.json cache arguments to C/C++ definitions.
# Load with -DCMAKE_PROJECT_INCLUDE=<absolute path to this file> so it also
# applies to TestDeploymentsProject and F Prime's separate sub-builds.
# Normal framework and consuming-project builds do not load this file.
include_guard(GLOBAL)

function(fprime_apply_ci_config_profile)
    set(PROFILE_SWITCHES
        FW_OBJECT_NAMES
        FW_OBJECT_REGISTRATION
        FW_QUEUE_REGISTRATION
        FW_PORT_TRACING
        FW_ENABLE_TEXT_LOGGING
        FW_SERIALIZABLE_TO_STRING
        FW_ASSERT_LEVEL
        FW_PORT_SERIALIZATION
        FW_ASSERTIONS_ALWAYS_ABORT
        FW_CMD_CHECK_RESIDUAL
    )
    get_cmake_property(CACHE_VARIABLES CACHE_VARIABLES)
    foreach(SWITCH IN LISTS CACHE_VARIABLES)
        if(NOT SWITCH MATCHES "^FW_")
            continue()
        endif()
        if(NOT SWITCH IN_LIST PROFILE_SWITCHES)
            message(FATAL_ERROR "Unsupported config-test switch: ${SWITCH}")
        endif()
        if(SWITCH STREQUAL "FW_ASSERT_LEVEL")
            set(VALID_VALUE "^[1-4]$")
        else()
            set(VALID_VALUE "^[01]$")
        endif()
        if(NOT "${${SWITCH}}" MATCHES "${VALID_VALUE}")
            message(FATAL_ERROR "Invalid config-test value for ${SWITCH}: '${${SWITCH}}'")
        endif()
        # Zero is a meaningful override. Do not test the value for truthiness.
        # This preserves toolchain/user flags and reaches both languages, child
        # directories, and generated sources through CMake's directory property.
        add_compile_definitions("${SWITCH}=${${SWITCH}}")
        message(STATUS "[config-test] ${SWITCH}=${${SWITCH}}")
    endforeach()
endfunction()

fprime_apply_ci_config_profile()
