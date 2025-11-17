

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DpContainer.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Dp.fpp"
  DEPENDS
    Utils_Hash
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Dp.fpp"
  DEPENDS
    STest
)
