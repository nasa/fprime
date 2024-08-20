# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/queued.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/QueuedTest.cpp"
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
  "${CMAKE_CURRENT_LIST_DIR}/queued.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/TestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/Tests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/TesterHandlers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/TesterHelpers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/PortTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/CmdTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/EventTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/TlmTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/ParamTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/TimeTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/InternalInterfaceTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/AsyncTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/AsyncTesterHelpers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/AsyncPortTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/AsyncCmdTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../tests/OverflowTests.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../types/FormalParamTypes.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../../utils/Utils.cpp"
)
# Needed for compiling files using Tester.hpp from outside the directory
include_directories("${CMAKE_CURRENT_LIST_DIR}/test/ut")
register_fprime_ut()
