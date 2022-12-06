####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RateGroupDriver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/RateGroupDriver.cpp"
)

register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/RateGroupDriver/RateGroupDriver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/RateGroupDriverTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/RateGroupDriverImplTester.cpp"
)
register_fprime_ut()
