####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/StreamCrossover.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/StreamCrossover.cpp"
)

register_fprime_module()

# Register the unit test build
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/StreamCrossover.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/StreamCrossoverTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/StreamCrossoverTestMain.cpp"
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
