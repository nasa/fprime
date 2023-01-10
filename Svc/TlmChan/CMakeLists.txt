####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TlmChan.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmChan.cpp"
)

register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/TlmChan/TlmChan.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Main.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
)
register_fprime_ut()

