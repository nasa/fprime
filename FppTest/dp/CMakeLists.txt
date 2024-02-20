set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/DpTest.fpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpTest.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterHelpers.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut()
