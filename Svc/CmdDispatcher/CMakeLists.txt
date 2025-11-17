
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
    "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImpl.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/CmdDispatcher.fpp"
)
### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/CmdDispatcher.fpp"
)
