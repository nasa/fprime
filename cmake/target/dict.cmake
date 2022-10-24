####
# dict.cmake:
#
# Dictionary target definition file. Used to define `dict` and `<MODULE>_dict` targets. Defined as
# a standard target pattern. This means that the following functions are defined:
#
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_dict'
####

# Dictionaries are per-deployment, a global variant does not make sense
function(dict_add_global_target)
endfunction()

# For now, using the by-product output of the autocode step
function(dict_add_deployment_target)
endfunction()

####
# Dict function `add_module_target`:
#
# Process the dictionary target on each module that is defined. Since the topology module is going to do the dictionary
# generation work for us, we just need to add a dependency on the module that contains the dictionary in its list of
# autocoder output files.
#
# - **MODULE:** name of the module
# - **TARGET:** name of the top-target (e.g. dict). Use ${MODULE_NAME}_${TARGET_NAME} for a module specific target
# - **SOURCE_FILES:** list of source file inputs from the CMakeList.txt setup
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(dict_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
    get_target_name(${TARGET} ${MODULE})
    run_ac_set("${SOURCES}" autocoder/fpp autocoder/ai_xml)
    set(DICTIONARY "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}TopologyAppDictionary.xml")
    foreach(FILE IN LISTS AC_GENERATED)
        if (FILE STREQUAL DICTIONARY)
            set_property(GLOBAL PROPERTY DICTIONARY_FILE "${DICTIONARY}")
            break()
        endif()
    endforeach()
endfunction(dict_add_module_target)
