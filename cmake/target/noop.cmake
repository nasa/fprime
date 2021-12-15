####
# cmake/target/noop.cmake:
#
# A target that does nothing, or a quick way to refresh cache only when necessary.
####

function(add_global_target)
    add_custom_target("noop")
endfunction()

function(add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
endfunction()

function(add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(add_module_target)
