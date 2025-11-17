

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DpTest.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpTest.fpp"
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpTestTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpTestTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpTest.fpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
