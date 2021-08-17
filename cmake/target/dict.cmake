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
# Function `dict`:
#
# Generate a dictionary from any *AppAi.xml file that we see
####
function(dictgen MODULE_NAME AI_XML DEPS)
  string(REGEX REPLACE "Ai.xml" "Dictionary.xml" DICT_XML "${AI_XML}")
  string(REGEX REPLACE "Ai.xml" "ID.csv" ID_CSV_XML "${AI_XML}")
  string(REGEX REPLACE "Ai.xml" "Ai_IDTableLog.txt" ID_LOG_XML "${AI_XML}")
  string(REGEX REPLACE "Ai.xml" "Ac" AC_BASE "${AI_XML}")
  string(REPLACE ";" ":" FPRIME_BUILD_LOCATIONS_SEP "${FPRIME_BUILD_LOCATIONS}")
  get_filename_component(DICT_XML_NAME ${DICT_XML} NAME)
  get_filename_component(ID_CSV_XML_NAME ${ID_CSV_XML} NAME)
  get_filename_component(ID_LOG_XML_NAME ${ID_LOG_XML} NAME)
  set(DICT_ROOT "${FPRIME_INSTALL_DEST}/${TOOLCHAIN_NAME}/dict")
  set(DICTIONARY_OUTPUT_FILE "${DICT_ROOT}/${DICT_XML_NAME}")

  add_custom_command(
      OUTPUT "${DICT_ROOT}/${DICT_XML_NAME}"
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_COMMAND} -E env PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils BUILD_ROOT="${FPRIME_BUILD_LOCATIONS_SEP}"
      FPRIME_AC_CONSTANTS_FILE="${FPRIME_AC_CONSTANTS_FILE}"
      PYTHON_AUTOCODER_DIR=${PYTHON_AUTOCODER_DIR}
      ${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/bin/codegen.py --build_root --xml_topology_dict ${AI_XML}
      COMMAND ${CMAKE_COMMAND} -E make_directory "${DICT_ROOT}"
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E copy ${DICT_XML_NAME} ${ID_LOG_XML_NAME} ${ID_CSV_XML_NAME} ${DICT_ROOT}
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E copy_directory commands "${DICT_ROOT}/commands"
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E copy_directory channels "${DICT_ROOT}/channels"
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E copy_directory events "${DICT_ROOT}/events"
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E remove ${DICT_XML_NAME} ${ID_CSV_XML_NAME} ${ID_LOG_XML_NAME} ${AC_BASE}.cpp ${AC_BASE}.hpp
      # Workaround for older versions of cmake (~v3.10) that can only delete a single directory with "remove_directory" command.
      # When bumping cmake versions combine all deletions into a single "cmake -E rm -rf" command.
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E remove_directory commands
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E remove_directory channels
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E remove_directory events
      DEPENDS ${DEPS}
  )

  # Return file for output
  set(DICTIONARY_OUTPUT_FILE "${DICTIONARY_OUTPUT_FILE}" PARENT_SCOPE) 
endfunction(dictgen)

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
    # Try to generate dictionaries for every AC input file
    foreach (AC_IN ${AC_INPUTS})
        # Only generate dictionaries on serializables or topologies
        if (AC_IN MATCHES ".*Topology.*\.xml$")
            fprime_ai_info("${AC_IN}" "${MODULE_NAME}")
            dictgen("${MODULE_NAME}" "${AC_IN}" "${AC_INPUTS};${MODULE_DEPENDENCIES};${MOD_DEPS};${FILE_DEPENDENCIES}")
            add_custom_target("${TARGET_NAME}" DEPENDS "${AC_IN}" "${DICTIONARY_OUTPUT_FILE}")
            add_dependencies("${MODULE_NAME}" "${TARGET_NAME}")
            add_dependencies("${GLOBAL_TARGET_NAME}" "${TARGET_NAME}")
        endif()
    endforeach()
endfunction(add_module_target)
