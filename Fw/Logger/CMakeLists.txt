####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Logger.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Logger.hpp"
  DEPENDS
    Fw_Types
)

# Rules based unit testing
# STest Includes for this UT
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FakeLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LoggerRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LoggerMain.cpp"
  DEPENDS
    STest
    Fw_Types
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
