set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Ds.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ArrayTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DsTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalArrayTest.cpp"
)

set(UT_MOD_DEPS
  STest
)

register_fprime_ut()
