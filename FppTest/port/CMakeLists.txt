# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/no_args_port.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/some_port.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/example_comp.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Example.cpp"
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
  "${CMAKE_CURRENT_LIST_DIR}/no_args_port.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/some_port.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/example_comp.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestMain.cpp"
)
register_fprime_ut()


