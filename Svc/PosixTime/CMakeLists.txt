####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
restrict_platforms(Posix)


register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/PosixTime.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PosixTime.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/PosixTime.hpp"
)
### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/PosixTime.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixTimeTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixTimeMain.cpp"
)
