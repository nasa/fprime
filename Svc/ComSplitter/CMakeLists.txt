####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ComSplitter.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComSplitter.fpp"
)
### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComSplitterTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComSplitterMain.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComSplitter.fpp"
)
