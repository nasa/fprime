
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Harness.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestAbsType.cpp"
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
  DEPENDS
    STest
)
