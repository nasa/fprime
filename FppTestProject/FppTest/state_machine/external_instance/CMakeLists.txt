

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SmTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/DeviceSm.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/HackSm.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SmTest.fpp"
  DEPENDS
    Fw_Sm
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SmTestTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/SmTestTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/DeviceSm.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/HackSm.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/SmTest.fpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
