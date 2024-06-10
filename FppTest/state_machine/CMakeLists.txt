set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SmTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SmTest.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/DeviceSm.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SmTest.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterHelpers.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/DeviceSm.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut()
