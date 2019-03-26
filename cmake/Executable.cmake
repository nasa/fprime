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
function(generate_executable EXECUTABLE_NAME AUTOCODER_INPUT_FILES SOURCE_FILES LINK_DEPS)
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
  
endfunction(generate_executable)
