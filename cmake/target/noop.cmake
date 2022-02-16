####
# cmake/target/noop.cmake:
#
# A target that does nothing, or a quick way to refresh cache only when necessary.
####

function(noop_add_global_target)
    add_custom_target("noop")
endfunction()

function(noop_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
endfunction()

function(noop_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(noop_add_module_target)
