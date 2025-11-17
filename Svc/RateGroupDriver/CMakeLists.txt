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
    "${CMAKE_CURRENT_LIST_DIR}/RateGroupDriver.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/RateGroupDriver.fpp"
)

### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/RateGroupDriverTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/RateGroupDriverImplTester.cpp"
  AUTOCODER_INPUTS
    "${FPRIME_FRAMEWORK_PATH}/Svc/RateGroupDriver/RateGroupDriver.fpp"
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
