
set (GEN_VERSION_FILE_CMD ${CMAKE_CURRENT_LIST_DIR}/version/generateVersionHeader.py "${CMAKE_BINARY_DIR}/version.hpp")

function(add_global_target TARGET_NAME)

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/version.hpp" __PHONY__ COMMAND ${CMAKE_COMMAND} -E chdir ${FPRIME_PROJECT_ROOT} ${GEN_VERSION_FILE_CMD})
    add_custom_target(${TARGET_NAME} ALL DEPENDS "${CMAKE_BINARY_DIR}/version.hpp" __PHONY__)

endfunction(add_global_target)

####
# Dict function `add_module_target`:
#
# Adds a module-by-module target for producing dictionaries. These dictionaries take the outputs
# from the autocoder and copies them into the correct directory. These outputs are then handled as
# part of the global `dict` target above.
#
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **GLOBAL_TARGET_NAME:** name of produced global target
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
# - **MOD_DEPS:** module dependencies of the target
####
function(add_module_target MODULE_NAME TARGET_NAME GLOBAL_TARGET_NAME AC_INPUTS SOURCE_FILES AC_OUTPUTS MOD_DEPS)

endfunction(add_module_target)
