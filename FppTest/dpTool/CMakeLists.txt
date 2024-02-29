set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpTool.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/DpTool.fpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DpTool.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterHelpers.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut()
