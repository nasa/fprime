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
    "${CMAKE_CURRENT_LIST_DIR}/PolyDb.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PolyDb.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/PolyDb.hpp"
)
### UTs ###
####
# WARNING:
#
# This UT does not use auto-generate GTest files. DO NOT emulate.
####

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/PolyDb.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PolyDbTester.cpp"
  UT_AUTO_HELPERS
)
