
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TestAbsType.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Harness.fpp"
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    STest
)
