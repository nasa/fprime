####
# dict.cmake:
#
# Dictionary target definition file. Used to define `dict` and `<MODULE>_dict` targets. Defined as
# a standard target pattern. This means that the following functions are defined:
#
# - `add_global_target`: adds a global target 'dict'
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_dict'
####
####
# Dict function `add_global_target`:
#
# Add target for the `dict` custom target. Dictionaries are built-in targets, but they are defined
# as custom targets. This handles the top-level dictionary target `dict` and registers the steps to
# perform the generation of the target.  TARGET_NAME should be set to `dict`.
#
# - **TARGET_NAME:** target name to be generated
####
function(add_global_target TARGET_NAME)
    add_custom_target(${TARGET_NAME} ALL)
    
endfunction(add_global_target)

####
# Dict function `add_module_target`:
#
# Process the dictionary target on each module that is defined. Since the topology module is going to do the dictionary
# generation work for us, we just need to add a dependency on the module that contains the dictionary in its list of
# autocoder output files.
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **GLOBAL_TARGET_NAME:** name of produced global target
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
# - **DEPENDENCIES:** module dependencies of the target
####
function(add_module_target MODULE TARGET GLOBAL_TARGET AC_INPUTS SOURCE_FILES AC_OUTPUTS DEPENDENCIES)
    set(DICTIONARY "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}TopologyAppDictionary.xml")
    foreach(FILE IN LISTS AC_OUTPUTS)
        if (FILE STREQUAL DICTIONARY)
            add_custom_target(
                ${TARGET}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/dict/"
                COMMAND ${CMAKE_COMMAND} -E copy ${DICTIONARY} "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/dict/"
                DEPENDS ${DICTIONARY} ${MODULE}
            )
            add_dependencies("${GLOBAL_TARGET}" "${TARGET}")
            break()
        endif()
    endforeach()
endfunction(add_module_target)
