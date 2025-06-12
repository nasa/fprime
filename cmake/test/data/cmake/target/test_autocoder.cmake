####
# target/test_recursion.cmake:
#
# This target sets up a test target for checking the right recursive dependencies come in
####
include(autocoder/autocoder)

function(test_autocoder_add_global_target TARGET)
endfunction(test_autocoder_add_global_target)

function(test_autocoder_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
endfunction(test_autocoder_add_deployment_target)

function(test_autocoder_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    run_ac_set(MODULE "${CMAKE_CURRENT_LIST_DIR}/../autocoder/test_target_autocoder.cmake")
endfunction(test_autocoder_add_module_target)
