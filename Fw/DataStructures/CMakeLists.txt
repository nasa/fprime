set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/DataStructures.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ArrayTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/DataStructuresTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ExternalArrayTest.cpp"
)

set(UT_MOD_DEPS
  STest
)

register_fprime_ut()
