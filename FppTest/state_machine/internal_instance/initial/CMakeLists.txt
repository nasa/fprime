set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Basic.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Choice.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Nested.fpp"
)
set(MOD_DEPS FppTest/state_machine/internal/harness)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/BasicTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ChoiceTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/NestedTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut()
