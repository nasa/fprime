####
# autocoder/deployment_validation.cmake
#
# An autocoder that performs validation steps on the deployment to ensure that it is well-formed. This will produce
# warning output when validation fails.
#
# Note: this autocoder is for validation purposes and DOES NOT produce any autocoded files.
#####
include(utilities)
include(autocoder/helpers)

autocoder_setup_for_multiple_sources()

####
# `deployment_validation_is_supported`:
#
# Required function, processes .fpp files
####
function(deployment_validation_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix(".fpp" "${AC_INPUT_FILE}") # No re-scan,  already done in FPP
endfunction (deployment_validation_is_supported)

####
# `deployment_validation_is_supported`:
#
# Required function, look for .fpp files defining a topology block and ensure the following equivalence:
#     ${PROJECT_NAME} == topology name
# No files are produced by this autocoder. It runs to validate items as part of the autocoder system.
#
####
function(deployment_validation_setup_autocode AC_INPUT_FILES)
    foreach(AC_FILE IN LISTS AC_INPUT_FILES)
        file(READ "${AC_FILE}" FILE_TEXT)
        # Does this file match the pattern (module { topology {} })
        if (NOT FPRIME_SKIP_PROJECT_NAME_VALIDATION AND
            FILE_TEXT MATCHES "^(.*\n)?[^\n#@]*topology +([a-zA-Z0-9_]+)" AND
            NOT CMAKE_MATCH_2 STREQUAL "${PROJECT_NAME}")
            message(WARNING
                "Cmake project name '${PROJECT_NAME}' does not match topology name '${CMAKE_MATCH_2}'\n"
                "    Project CMakeLists.txt: ${PROJECT_SOURCE_DIR}/CMakeLists.txt\n"
                "    Topology FPP model: ${AC_FILE}\n"
            )
        endif()
    endforeach()
    # This autocoder specifically does not produce any autocoder output
    set(AUTOCODER_GENERATED "" PARENT_SCOPE)
endfunction(deployment_validation_setup_autocode)
