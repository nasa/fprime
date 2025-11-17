####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoded files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.cpp"
)

### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferRepeaterTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferRepeaterTester.cpp"
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
