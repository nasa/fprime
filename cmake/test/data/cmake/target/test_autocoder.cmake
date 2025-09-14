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
    run_ac_set("${MODULE}" "autocoder/test_target_autocoder")
    # Use the variable from this run as set by the autocoder
    add_custom_target("${MODULE}_test_autocode" DEPENDS "${AUTOCODER_GENERATED_OTHER}")
    add_dependencies("${MODULE}" "${MODULE}_test_autocode")
endfunction(test_autocoder_add_module_target)
