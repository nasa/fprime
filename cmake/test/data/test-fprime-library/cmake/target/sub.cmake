####
# target/sub.cmake:
#
# This target sets up a sub-build target for testing purposes.
####
function(sub_add_global_target TARGET)
    add_custom_target("${TARGET}"
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/sub-test"
        COMMAND ${CMAKE_COMMAND} -E touch "${FPRIME_BINARY_DIR}/sub-test"
    )
endfunction(sub_add_global_target)

function(sub_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
endfunction(sub_add_deployment_target)

function(sub_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
endfunction(sub_add_module_target)
