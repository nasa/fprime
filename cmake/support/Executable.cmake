####
# Executable.cmake:
#
# Contains the code required to make executables and deployments. This is a unified entry point
# for any binary that will be built with the F prime CMake system.
#
# Defines the following CMake functions:
#
# generate_executable(EXECUTABLE_NAME AUTOCODER_INPUT_FILES SOURCE_FILES LINK_DEPS)
#
# EXECUTABLE_NAME: name of executable to generate
# AUTOCODER_INPUT_FILES: list of autocoder input files to use F prime autocoding
# SOURCE_FILES: files used to build the source of this executable. Note: most sources come in
#               as link dependencies
# LINK_DEPS: non-automatic dependencies needed at link time. Most F prime components and ports
#            are found automatically.
#
####
include("${CMAKE_CURRENT_LIST_DIR}/Module.cmake")
####
# Add "dict" Target:
#
# This target is the target for generating the dictionaries. This will have the output dictionaries registered to
# it and make it possible to generate the dictionaries out-of-band from the compile/build.
####
function(add_dict_target)
    # If we are generating python dictionaries, then we need to copy the outputs
    if (GENERATE_HERITAGE_PY_DICT)
        add_custom_target(
            dict
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/dict/serializable ${CMAKE_SOURCE_DIR}/py_dict/serializable
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/py_dict/serializable/__init__.py
        )
    # Otherwise just add an empty target
    else()
        add_custom_target(dict)   
    endif()
endfunction(add_dict_target)

####
# Generate Executable:
#
# Top-level executable generation.
# \param EXECUTABLE_NAME: name of executable to be generated.
# \param AUTOCODER_INPUT_FILES: list of auto-coder files input into this executable
# \param SOURCE_FILES: source files for this executable
# \param LINK_DEPS: specified module-level link dependencies
####
function(generate_executable EXECUTABLE_NAME AUTOCODER_INPUT_FILES SOURCE_FILES LINK_DEPS)
  add_dict_target()
  message(STATUS "Adding executable: ${EXECUTABLE_NAME}")
  add_executable(
      "${EXECUTABLE_NAME}"
      "${EMPTY_C_SRC}" # Added to suppress warning if module only has autocode
  )
  # Generate module from sources
  generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}")

  # Sets MODULE_NAME to unique name based on path
  get_module_name(${CMAKE_CURRENT_LIST_DIR})
  target_link_libraries("${EXECUTABLE_NAME}" "${MODULE_NAME}")

  #Force top-level to depend on dicts and make the executable run the "dict" command
  add_dependencies(dict "${MODULE_NAME}_DICT")
  add_dependencies("${EXECUTABLE_NAME}" dict)
endfunction(generate_executable)
