####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ActiveLogger.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/ActiveLoggerImpl.cpp"
)

register_fprime_module()
### UTs ###
set(UT_MOD_DEPS
    Os_Stubs
)
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/ActiveLogger/ActiveLogger.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ActiveLoggerTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ActiveLoggerImplTester.cpp"
)
register_fprime_ut()
