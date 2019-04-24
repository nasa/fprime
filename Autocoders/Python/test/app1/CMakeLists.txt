# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DuckComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Msg1PortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/DuckTopologyAppAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/DuckImpl.cpp"
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
  "${FPRIME_CORE_DIR}/Autocoders/Python/test/app1/DuckComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Top.cpp"
)

#register_fprime_ut()


