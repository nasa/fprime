####
# testimpl.cmake:
#
# This target invokes the unit test code template generation available as part of the fprime autocoder. It implements
# the target interface described here: [Targets](Targets.md).
#
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_test_impl'
#
# Note: this should be registered as a ut target
####

####
# testimpl `add_global_target`:
#
# Does nothing. Prevents default target.
####
function(add_global_target TARGET)
endfunction()

####
# testimpl `add_deployment_target`:
#
# Does nothing. Prevents default "roll-up" target.
####
function(add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
endfunction()

####
# testimpl function `add_module_target`:
#
# Adds a module-by-module target for producing ut implementations. Take in the Ai.xml file and produces a set of test
# templates. Note: the testimpl autocoder path is: fpp (already run by the main build) followed by ai-ut-impl.
#
# - **MODULE:** name of the module
# - **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
# - **SOURCE_FILES:** list of source file inputs from the CMakeList.txt setup
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(add_module_target MODULE TARGET SOURCE_FILES DEPENDENCIES)
    get_target_name(${TARGET} ${MODULE})
    # Try to generate dictionaries for every AC input file
    if (NOT TARGET "${TARGET_MOD_NAME}")
        run_ac_set("${SOURCE_FILES}" INFO_ONLY autocoder/fpp autocoder/ai-ut-impl)
        add_custom_target("${TARGET_MOD_NAME}" DEPENDS ${AC_GENERATED})
    endif()
endfunction(add_module_target)
