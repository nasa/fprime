####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/BlockDriver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/BlockDriver.cpp"
)

register_fprime_module()

### Unit Tests ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/BlockDriver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BlockDriverTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/BlockDriverTester.cpp"
)
set(UT_MOD_DEPS
  STest
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()