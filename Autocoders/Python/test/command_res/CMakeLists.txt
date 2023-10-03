# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Test1ComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Test2PortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Test1ComponentImpl.cpp"
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
  "${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/test/command_res/Test1ComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/command_resTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
register_fprime_ut()


