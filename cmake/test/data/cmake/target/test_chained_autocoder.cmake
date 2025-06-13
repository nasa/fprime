####
# target/test_chained_autocoder.cmake:
#
# This target sets up a test target for checking that chained autocoders work correctly
####
include(autocoder/autocoder)

function(test_chained_autocoder_add_global_target TARGET)
endfunction(test_chained_autocoder_add_global_target)

function(test_chained_autocoder_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
endfunction(test_chained_autocoder_add_deployment_target)

function(test_chained_autocoder_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    # Run both autocoders in sequence: target autocoder first, then chained autocoder
    run_ac_set("${MODULE}" "autocoder/test_target_autocoder" "autocoder/test_chained_autocoder")
    
    # Use the variable from this run as set by the autocoders
    add_custom_target("${MODULE}_test_chained_autocode" DEPENDS "${AUTOCODER_GENERATED_OTHER}")
    add_dependencies("${MODULE}" "${MODULE}_test_chained_autocode")
endfunction(test_chained_autocoder_add_module_target)
