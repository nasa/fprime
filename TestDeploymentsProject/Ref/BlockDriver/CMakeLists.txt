####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BlockDriver.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BlockDriver.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/BlockDriver.hpp"
)

### Unit Tests ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BlockDriver.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BlockDriverTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BlockDriverTester.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()