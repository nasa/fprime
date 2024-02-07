####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Linux Darwin)

set(MOD_DEPS Os)
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.cpp"
)
register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxUartDriverTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxUartDriverTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxUartDriverTesterHelpers.cpp"

)
register_fprime_ut()

