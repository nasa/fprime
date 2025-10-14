set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/A.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/AComponentAc.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/empty.fpp"
)
set(MOD_DEPS
  Fw/Comp
  Fw/Types
  Os
)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/AGTestBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ATestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ATester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ATesterBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ATesterHelpers.cpp"
)

set(UT_MOD_DEPS
  FppTest/topology/ports
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
