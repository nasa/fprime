

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.hpp"
  DEPENDS
    Fw_Logger
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ByteStreamBufferAdapterTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ByteStreamBufferAdapterTester.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
