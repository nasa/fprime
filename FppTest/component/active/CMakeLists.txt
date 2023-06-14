# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/active.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/port_index_enums.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ActiveTest.cpp"
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

# Declare dependencies on test modules
set(UT_MOD_DEPS
  Fw/Test
  STest
)

# Add unit test directory
# UT_SOURCE_FILES: Sources for unit test
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/active.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/port_index_enums.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/TestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/Tests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/TesterHandlers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/TesterHelpers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/PortTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/CmdTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/EventTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/TlmTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/ParamTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/TimeTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/InternalInterfaceTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/AsyncTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/AsyncTesterHandlers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/AsyncTesterHelpers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../common/AsyncPortTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../../types/FormalParamTypes.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../../utils/Utils.cpp"
)
register_fprime_ut()
