set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.cpp"
)

set(MOD_DEPS
    "Fw/Logger"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ByteStreamBufferAdapterTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ByteStreamBufferAdapterTester.cpp"
)

set(UT_MOD_DEPS
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()
