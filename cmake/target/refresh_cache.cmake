####
# cmake/target/refresh_cache.cmake:
#
# A target that does nothing, or a quick way to refresh cache only when necessary.
####

function(refresh_cache_add_global_target)
    add_custom_target("refresh_cache")
endfunction()

function(refresh_cache_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
endfunction()

function(refresh_cache_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
endfunction(refresh_cache_add_module_target)
