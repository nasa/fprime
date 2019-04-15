# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(AUTOCODER_INPUT_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TestComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Test2PortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/QuaternionSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestPtComponentAi.xml"
)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TestPrmSourceImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestCommandSourceImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestTelemRecvImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestCommandImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestLogRecvImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestPtSourceImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)

generate_module("${AUTOCODER_INPUT_FILES}" "${SOURCE_FILES}")

# Sets MODULE_NAME to unique name based on path
get_module_name(${CMAKE_CURRENT_LIST_DIR})

# Depends on a component defined in another module
add_dependencies(
  ${MODULE_NAME}
  "Autocoders_test_command_tester"
  "Autocoders_test_log_tester"
  "Autocoders_test_param_tester"
  "Autocoders_test_telem_tester"
  "Autocoders_test_time_tester"
)


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
