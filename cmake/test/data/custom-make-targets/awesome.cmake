####
# awesome.cmake:
#
# Awesome target for testing custom targets.
####

####
# Awesome function `add_global_target`:
# - **TARGET_NAME:** target name to be generated
####
function(add_global_target TARGET_NAME)
    add_custom_target(
        ${TARGET_NAME} ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/awesome
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/awesome/everything_is_awesome
    )
endfunction(add_global_target)
####
# Awesome function `add_module_target`:
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
####
function(add_module_target MODULE_NAME TARGET_NAME AC_INPUTS SOURCE_FILES AC_OUTPUTS)
    add_custom_target(
        ${TARGET_NAME} ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/awesome
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/awesome/${MODULE_NAME}_is_awesome
    )
endfunction(add_module_target)
