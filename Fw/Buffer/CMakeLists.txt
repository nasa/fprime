####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####


register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Buffer.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Buffer.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Buffer.hpp"
  DEPENDS
    Fw_Types
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestBuffer.cpp"
)
