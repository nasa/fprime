####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Time.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Time.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TimeInterval.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Time.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/TimeInterval.hpp"
)
### UTs ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TimeIntervalTester.cpp"
  DEPENDS
    Fw_Time
    Fw_Types
)
