# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/NoArgPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/SomePortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ExampleComponentImpl.cpp"
)

register_fprime_module()

# Sets MODULE_NAME to unique name based on path
get_module_name(${CMAKE_CURRENT_LIST_DIR})

# Exclude test module from all build
set_target_properties(
  ${MODULE_NAME}
  PROPERTIES
  EXCLUDE_FROM_ALL TRUE
)

# Add unit test directory
# UT_SOURCE_FILES: Sources for unit test
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ExampleComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/noargportTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/noargportTestMain.cpp"
)
register_fprime_ut()


