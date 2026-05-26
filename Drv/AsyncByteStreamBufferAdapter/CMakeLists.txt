

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.hpp"
  DEPENDS
    Fw_Logger
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/AsyncByteStreamBufferAdapterTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/AsyncByteStreamBufferAdapterTester.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
