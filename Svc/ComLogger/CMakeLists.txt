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
    "${CMAKE_CURRENT_LIST_DIR}/ComLogger.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComLogger.fpp"
)
### UTs ###

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComLoggerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComLoggerMain.cpp"
  AUTOCODER_INPUTS
    "${FPRIME_FRAMEWORK_PATH}/Svc/ComLogger/ComLogger.fpp"
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
