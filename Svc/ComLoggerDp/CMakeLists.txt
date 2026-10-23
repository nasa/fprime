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
    "${CMAKE_CURRENT_LIST_DIR}/ComLoggerPorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/ComLoggerDp.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ComLoggerDp.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ComLoggerDp.hpp"
)
### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComLoggerPorts.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/ComLoggerDp.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComLoggerDpTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComLoggerDpTestMain.cpp"
  UT_AUTO_HELPERS
)
