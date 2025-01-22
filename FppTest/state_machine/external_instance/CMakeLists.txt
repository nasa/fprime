set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SmTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SmTest.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/DeviceSm.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/HackSm.cpp"
)
set(MOD_DEPS Fw/Sm)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SmTest.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SmTestTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/SmTestTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/DeviceSm.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/HackSm.cpp"
)
set(UT_MOD_DEPS STest)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
