# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TestComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/QuaternionSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestTelemImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestTelemRecvImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
set(MOD_DEPS
  Autocoders/Python/test/telem_tester
  Autocoders/Python/test/time_tester
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
  "${FPRIME_CORE_DIR}/Autocoders/Python/test/tlm_onchange/TestComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)

register_fprime_ut()


