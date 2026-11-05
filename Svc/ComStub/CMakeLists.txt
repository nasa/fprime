####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComStub.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ComStub.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ComStub.hpp"
  DEPENDS
    Fw_Logger
)

register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComStub.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComStubTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComStubTestMain.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
