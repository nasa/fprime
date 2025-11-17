

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
    "${CMAKE_CURRENT_LIST_DIR}/Basic.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Choice.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Nested.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    STest
)
