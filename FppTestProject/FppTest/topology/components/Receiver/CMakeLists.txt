set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Receiver.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Receiver.fpp"
)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Receiver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ReceiverTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ReceiverTester.cpp"
)

set(UT_MOD_DEPS
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
