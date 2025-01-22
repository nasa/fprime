####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PrmDb.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/PrmDbImpl.cpp"
)

register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
  "${FPRIME_FRAMEWORK_PATH}/Svc/PrmDb/PrmDb.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PrmDbTester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/PrmDbImplTester.cpp"
)

set(UT_MOD_DEPS
    Os_File_Test_Stub
    Os_File_Stub
)
choose_fprime_implementation(Os/File Os/File/None)
register_fprime_ut()

