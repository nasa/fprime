# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MsgPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/VoidPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/VoidArgComponentAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/App2VoidArgImpl.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")

# Sets MODULE_NAME to unique name based on path
get_module_name(${CMAKE_CURRENT_LIST_DIR})

# Exclude test module from all build
set_target_properties(
  ${MODULE_NAME}
  PROPERTIES
  EXCLUDE_FROM_ALL TRUE
)

# Add unit test directory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
