####
# Executable.cmake:
#
# Contains the code required to make executables and deployments. This is a unified entry point
# for any binary that will be built with the F prime CMake system.
#
####
include("${CMAKE_CURRENT_LIST_DIR}/Module.cmake")
####
# Function `add_dict_target`:
#
# This target is the target for generating the dictionaries. This will have the output dictionaries registered to
# it and make it possible to generate the dictionaries out-of-band from the compile/build.
#
####
function(add_dict_target)
    # If we are generating python dictionaries, then we need to copy the outputs
    if (CMAKE_BUILD_TYPE STREQUAL "TESTING")
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
# Function `generate_executable:`
#
# Top-level executable generation. Core allows for generarion of UT specifics without affecting API.
#
# - **EXECUTABLE_NAME:** name of executable to be generated.
# - **SOURCE_FILES_INPUT:** source files for this executable, split into AC and normal sources
# - **DEPS_INPUT:** specified module-level dependencies
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
  # CMake object type
  if (NOT DEFINED FPRIME_OBJECT_TYPE)
      set(FPRIME_OBJECT_TYPE "Executable")
  endif()
  # Install the executable
  generate_module(${EXECUTABLE_NAME} "${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}" "${LINK_DEPS}" "${MOD_DEPS}")
  # Link library list output on per-module basis
  # Install the executable, if not excluded and not testing
  get_target_property(IS_EXCLUDE_FROM_ALL "${EXECUTABLE_NAME}" "EXCLUDE_FROM_ALL")
  if ("${IS_EXCLUDE_FROM_ALL}" STREQUAL "IS_EXCLUDE_FROM_ALL-NOTFOUND" AND
      NOT CMAKE_BUILD_TYPE STREQUAL "TESTING") 
      install(TARGETS "${EXECUTABLE_NAME}"
          RUNTIME DESTINATION "bin/${PLATFORM}"
          LIBRARY DESTINATION "lib/${PLATFORM}"
          ARCHIVE DESTINATION "lib/static/${PLATFORM}"
      )
  endif()
  if (CMAKE_DEBUG_OUTPUT)
	  print_dependencies(${EXECUTABLE_NAME})
  endif()
endfunction(generate_executable)
