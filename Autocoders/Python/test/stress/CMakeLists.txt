# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TestComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Test2PortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/QuaternionSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestPtComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestPrmSourceImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestCommandSourceImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestTelemRecvImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestCommandImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestLogRecvImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TestPtSourceImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
set(MOD_DEPS
  Autocoders/Python/test/telem_tester
  Autocoders/Python/test/log_tester
  Autocoders/Python/test/param_tester
  Autocoders/Python/test/command_tester
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
  "${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/test/stress/TestPtComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
register_fprime_ut()

# ac.ini needs to be copied into Autocode directory
if (CMAKE_BUILD_TYPE STREQUAL "TESTING" AND NOT __FPRIME_NO_UT_GEN__)
  add_custom_target(py_ac_test_stress_ac_ini
     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_LIST_DIR}/Autocode
     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/ac.ini ${CMAKE_CURRENT_LIST_DIR}/Autocode
  )
  add_dependencies(${MODULE_NAME}_ut_exe py_ac_test_stress_ac_ini)
endif()


