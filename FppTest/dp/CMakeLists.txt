set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/DpTest.fpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpTest.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpTestTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DpTestTester.cpp"
)
set(UT_MOD_DEPS STest)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
