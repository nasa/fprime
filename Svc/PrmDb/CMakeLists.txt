####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PrmDbComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/PrmDbImpl.cpp"
)

register_fprime_module()

### UTs ###
set(UT_MOD_DEPS
    Os_Stubs
)
set(UT_SOURCE_FILES
  "${FPRIME_CORE_DIR}/Svc/PrmDb/PrmDbComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PrmDbTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PrmDbImplTester.cpp"
)
register_fprime_ut()