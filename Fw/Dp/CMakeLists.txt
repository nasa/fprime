

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Dp.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/DpContainer.cpp"
  DEPENDS
    Utils_Hash
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Dp.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
  DEPENDS
    STest
)
