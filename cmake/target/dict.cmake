####
# dict.cmake:
#
# Dictionary target definition file. Used to define `dict` and `<MODULE>_dict` targets. Defined as
# a standard target pattern. This means that the following functions are defined:
#
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_dict'
####

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
####
function(add_module_target MODULE TARGET SOURCES)
    get_target_name(${TARGET} ${MODULE})
    run_ac_set("${SOURCES}" INFO_ONLY autocoder/fpp INFO_ONLY autocoder/ai-xml)
    set(DICTIONARY "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}TopologyAppDictionary.xml")
    foreach(FILE IN LISTS AC_GENERATED)
        if (FILE STREQUAL DICTIONARY)
            add_custom_target(${TARGET_MOD_NAME}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/dict/"
                COMMAND ${CMAKE_COMMAND} -E copy ${DICTIONARY} "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/dict/"
                DEPENDS ${DICTIONARY} ${MODULE}
            )
            add_dependencies("${TARGET}" "${TARGET_MOD_NAME}")
            break()
        endif()
    endforeach()
endfunction(add_module_target)
