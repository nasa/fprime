####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoded files
# MOD_DEPS: (optional) module dependencies
#
####


register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.hpp"
)

### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferRepeaterTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferRepeaterTester.cpp"
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
