####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####


register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComLogger.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ComLogger.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ComLogger.hpp"
)
### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComLogger.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComLoggerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComLoggerMain.cpp"
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
