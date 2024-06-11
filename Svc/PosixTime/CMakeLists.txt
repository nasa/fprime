####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
restrict_platforms(Posix)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PosixTime.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/PosixTime.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PosixTime.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixTimeTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixTimeMain.cpp"
)
register_fprime_ut()
