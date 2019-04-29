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
# Function `setup_module_dicts`:
#
# Creates a dictionary target for the module, that is then added to the "dict" and "module"
# targets.
#
# - **MOD_NAME:** name of module being processed
# - **AI_XML:** AI_XML that is generating dictionaries
# - **DICT_INPUTS:** inputs from auto-coder, used to trigger dictionary generation
####
function(setup_module_dicts MOD_NAME TARGET_NAME AI_XML_FULL DICT_INPUTS)
    # UTs don't supply directories
    if (UT_BUILD)
        return()
    endif()
    get_filename_component(AI_XML ${AI_XML_FULL} NAME_WE)
	set(AI_DICT_NAME "${AI_XML}_${TARGET_NAME}")
	# Add the dictionary target for this module, if it doesn't already exist
	if (NOT TARGET ${TARGET_NAME})
	    add_custom_target(${TARGET_NAME})
		if (CMAKE_DEBUG_OUTPUT)
		    message(STATUS "\tAdding Dict Target: ${MOD_NAME}_dict")
		endif()
	endif()
	add_custom_target(${AI_DICT_NAME} DEPENDS ${DICT_INPUTS})
	# Add dependencies upstream
	add_dependencies(${AI_DICT_NAME} ${CODEGEN_TARGET})
	add_dependencies(${TARGET_NAME} ${AI_DICT_NAME})
endfunction(setup_module_dicts)
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
    # If we are generating python dictionaries, then we need to copy the outputs
    if (UT_BUILD)
        return()
    elseif (GENERATE_HERITAGE_PY_DICT)
        add_custom_target(
            ${TARGET_NAME} ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/dict/serializable ${CMAKE_SOURCE_DIR}/py_dict/serializable
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/py_dict/serializable/__init__.py
        )
    # Otherwise just add an empty target
    else()
        add_custom_target(${TARGET_NAME} ALL)
    endif()
endfunction(add_global_target)
####
# Dict function `add_module_target`:
#
# Adds a module-by-module target for procducing dictionaries. These dictionaries take the outputs
# from the autocoder and copies them into the correct directory. These outputs are then handled as
# part of the global `dict` target above.
#
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
####
function(add_module_target MODULE_NAME TARGET_NAME AC_INPUTS SOURCE_FILES AC_OUTPUTS)
    # Try to generate dictionaries for every AC input file
    foreach (AC_IN ${AC_INPUTS})
    	# Only generate dictionaries on serializables or topologies
    	if (AC_IN MATCHES ".*Serializable.*\.xml$" OR AC_IN MATCHES ".*Topology.*\.xml$")
    	    setup_module_dicts("${MODULE_NAME}" "${TARGET_NAME}" "${AC_IN}" "${AC_OUTPUTS}")
    	endif()
    endforeach()
endfunction(add_module_target)