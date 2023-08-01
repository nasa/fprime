# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/empty.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Empty.cpp"
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
  "${CMAKE_CURRENT_LIST_DIR}/empty.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterHelpers.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
register_fprime_ut()
