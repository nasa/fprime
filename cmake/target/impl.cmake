####
# impl.cmake:
#
# This target invokes the code template generation available as part of the fprime autocoder. It implements
# the target interface described here: [Targets](Targets.md).
#
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_impl'
####

####
# Impl function `add_module_target`:
#
# Adds a module-by-module target for producing implementations. Take in the Ai.xml file and produces a set template
# files. Note: the impl autocoder path is: fpp (already run by the main build) followed by ai-impl.
#
# - **MODULE:** name of the module
# - **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
# - **SOURCE_FILES:** list of source file inputs from the CMakeList.txt setup
####
function(add_module_target MODULE TARGET SOURCE_FILES)
    get_target_name(${TARGET} ${MODULE})
    run_ac_set("${SOURCE_FILES}" INFO_ONLY autocoder/fpp autocoder/ai-impl)
    add_custom_target("${TARGET_MOD_NAME}" DEPENDS ${AC_GENERATED})
endfunction(add_module_target)
