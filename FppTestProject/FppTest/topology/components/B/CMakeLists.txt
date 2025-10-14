set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/B.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/BComponentAc.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/empty.fpp"
)
set(MOD_DEPS
  Fw/Comp
  Fw/Types
  Os
)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/../../../../../Fw/Comp/ActiveComponentBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../../../../../Fw/Comp/QueuedComponentBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BGTestBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BTesterBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BTesterHelpers.cpp"
)

set(UT_MOD_DEPS
  FppTest/topology/ports
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
