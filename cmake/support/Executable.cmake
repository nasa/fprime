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
    if (UT_BUILD)
        return()
    elseif (GENERATE_HERITAGE_PY_DICT)
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
# Top-level executable generation. Core allows for generarion of UT specifics without affecting API.
# \param EXECUTABLE_NAME: name of executable to be generated.
# \param SOURCE_FILES_INPUT: source files for this executable, split into AC and normal sources
# \param DEPS_INPUT: specified module-level dependencies
####
function(generate_executable EXECUTABLE_NAME SOURCE_FILES_INPUT DEPS_INPUT)
  # Set the following variables from the existing SOURCE_FILES and LINK_DEPS by splitting them into
  # their separate peices. 
  #
  # AUTOCODER_INPUT_FILES = *.xml and *.txt in SOURCE_FILES_INPUT, fed to auto-coder
  # SOURCE_FILES = all other items in SOURCE_FILES_INPUT, set as compile-time sources
  # LINK_DEPS = -l link flags given to DEPS_INPUT
  # MOD_DEPS = All other module inputs DEPS_INPUT
  split_source_files("${SOURCE_FILES_INPUT}")
  split_dependencies("${DEPS_INPUT}")

  message(STATUS "Adding executable: ${EXECUTABLE_NAME}")
  add_executable(
      "${EXECUTABLE_NAME}"
      "${SOURCE_FILES}" # Added to suppress warning if module only has autocode
  )
  # Setup a dictionary target, and generate an input module
  add_dict_target()
  generate_module(${EXECUTABLE_NAME} "${AUTOCODER_INPUT_FILES}" "${LINK_DEPS}" "${MOD_DEPS}")
  # Add dependencies to the 'dict' target, if not a UT build
  if (UT_BUILD)
      add_dependencies(dict "${MODULE_NAME}_dict")
      add_dependencies("${EXECUTABLE_NAME}" dict)
  endif()
endfunction(generate_executable)