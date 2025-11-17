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
    "${CMAKE_CURRENT_LIST_DIR}/TlmChan.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/TlmChan.cpp"
)

### UTs ###

register_fprime_ut(
  SOURCES
    "${FPRIME_FRAMEWORK_PATH}/Svc/TlmChan/TlmChan.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmChanMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TlmChanTester.cpp"
)

