
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
    "${CMAKE_CURRENT_LIST_DIR}/CmdDispatcher.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImpl.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcher.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImpl.hpp"
)
### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/CmdDispatcher.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTester.cpp"
)
