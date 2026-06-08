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
    "${CMAKE_CURRENT_LIST_DIR}/PassiveConsoleTextLogger.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ConsoleTextLoggerImplCommon.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ConsoleTextLoggerImpl.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/PassiveTextLogger.hpp"
  DEPENDS
    Fw_Logger
)
