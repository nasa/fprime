####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Time.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Time.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TimeInterval.cpp"
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
