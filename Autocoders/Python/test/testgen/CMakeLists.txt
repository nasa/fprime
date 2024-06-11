# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathOpPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathResultPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentImpl.cpp"
)
register_fprime_module()

# Sets MODULE_NAME to unique name based on path
get_module_name(${CMAKE_CURRENT_LIST_DIR})

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/testgenTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/testgenTestMain.cpp"
)

register_fprime_ut()
