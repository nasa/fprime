
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpTest.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DpTest.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/DpTest.hpp"
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/DpTest.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpTestTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpTestTester.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
