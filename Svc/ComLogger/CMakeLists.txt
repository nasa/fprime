####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ComLogger.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ComLogger.cpp"
)

register_fprime_module()
### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/ComLogger/ComLogger.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComLoggerTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ComLoggerMain.cpp"
)
register_fprime_ut()
