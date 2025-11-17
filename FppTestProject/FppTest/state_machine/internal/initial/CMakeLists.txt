

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Basic.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Choice.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Nested.fpp"
  DEPENDS
    FppTest/state_machine/internal/harness
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
