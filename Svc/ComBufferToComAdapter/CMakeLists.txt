####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
####
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComBufferToComAdapter.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ComBufferToComAdapter.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ComBufferToComAdapter.hpp"
)
### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComBufferToComAdapter.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComBufferToComAdapterTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComBufferToComAdapterTestMain.cpp"
)
