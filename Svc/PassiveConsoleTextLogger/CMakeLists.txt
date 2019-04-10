####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/ConsoleTextLoggerImplCommon.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Stub/PrintfLoggerImplStub.cpp"
)
set(MOD_DEPS
  Svc/PassiveTextLogger
)

register_fprime_module()
