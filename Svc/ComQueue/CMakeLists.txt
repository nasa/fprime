####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_library(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ComQueue.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComQueue.fpp"
  DEPENDS
    Fw_Types
    Utils_Types
    Fw_Buffer
)

### UTs ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComQueueTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComQueueTester.cpp"
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ComQueue.fpp"
  UT_AUTO_HELPERS
)

set (UT_TARGET_NAME "${FPRIME_CURRENT_MODULE}_ut_exe")
if (TARGET "${UT_TARGET_NAME}")
    target_compile_options("${UT_TARGET_NAME}" PRIVATE -Wno-conversion)
endif()
