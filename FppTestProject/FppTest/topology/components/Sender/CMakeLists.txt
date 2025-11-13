set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Sender.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Sender.fpp"
)
set(MOD_DEPS
  Fw/Comp
  Fw/Types
  Os
)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Sender.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ATestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ATester.cpp"
)

set(UT_MOD_DEPS
  FppTest/topology/ports
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
