####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ActiveTextLoggerComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/ActiveTextLoggerImpl.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogFile.cpp"
)

register_fprime_module()
### UTs ###
set(UT_MOD_DEPS
    Os_Stubs
)
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/ActiveTextLogger/ActiveTextLoggerComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/ActiveTextLoggerTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
)
register_fprime_ut()