

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SmTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SmTest.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/DeviceSm.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/HackSm.cpp"
  DEPENDS
    Fw/Sm
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SmTest.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SmTestTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SmTestTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/DeviceSm.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/HackSm.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
