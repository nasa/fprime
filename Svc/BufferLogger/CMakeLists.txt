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
    "${CMAKE_CURRENT_LIST_DIR}/BufferLogger.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BufferLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BufferLoggerFile.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/BufferLogger.hpp"
)

### UTS ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/BufferLogger.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferLoggerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Logging.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Errors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Health.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/BufferLoggerMain.cpp"
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
