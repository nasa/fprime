

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Basic.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Choice.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Nested.fpp"
  DEPENDS
    FppTest_state_machine_internal_harness
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BasicTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ChoiceTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/NestedTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    STest
)
