####
# target/test_chained_autocoder.cmake:
#
# This target sets up a test target for checking that chained autocoders work correctly
####
include(autocoder/autocoder)

function(test_rerun_autocoder_add_global_target TARGET)
endfunction(test_rerun_autocoder_add_global_target)

function(test_rerun_autocoder_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
endfunction(test_rerun_autocoder_add_deployment_target)

function(test_rerun_autocoder_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    run_ac_set("${MODULE}" "autocoder/test_build_autocoder")
    add_custom_target(
        "${MODULE}_test_rerun_autocoder"
        DEPENDS "${AUTOCODER_GENERATED_BUILD_SOURCES}"
        COMMAND "${CMAKE_COMMAND}" -E touch "${CMAKE_CURRENT_BINARY_DIR}/test-rerun-autocoder.txt"
        COMMENT "Creating test-rerun-autocoder.txt"
    )
    add_dependencies("${MODULE}" "${MODULE}_test_rerun_autocoder")
endfunction(test_rerun_autocoder_add_module_target)
