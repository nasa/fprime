####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoded files
# MOD_DEPS: (optional) module dependencies
#
####

set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.cpp"
)

register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/BufferRepeater.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferRepeaterTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferRepeaterTester.cpp"
)
register_fprime_ut()
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
