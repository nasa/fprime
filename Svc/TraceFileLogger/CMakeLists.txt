####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TraceFileLogger.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/TraceFileLogger.cpp"
)

register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TraceLogger.fpp"
   "${CMAKE_CURRENT_LIST_DIR}/test/ut/TraceFileLoggerTester.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/test/ut/TraceFileLoggerTestMain.cpp"
)

set(UT_MOD_DEPS
  Fw/Buffer
  STest
)

set(UT_AUTO_HELPERS ON)
register_fprime_ut()

