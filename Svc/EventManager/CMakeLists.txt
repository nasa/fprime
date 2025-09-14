####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####


register_fprime_library(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/EventManager.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/EventManager.fpp"
)

### UTs ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/EventManagerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/EventManagerTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/EventManager.fpp"
)
set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
