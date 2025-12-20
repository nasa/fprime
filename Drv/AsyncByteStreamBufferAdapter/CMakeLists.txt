set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.cpp"
)

set(MOD_DEPS
    "Fw/Logger"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/AsyncByteStreamBufferAdapterTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/AsyncByteStreamBufferAdapterTester.cpp"
)

set(UT_MOD_DEPS
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
