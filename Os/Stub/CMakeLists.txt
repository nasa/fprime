####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_custom_target("${FPRIME_CURRENT_MODULE}")
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
)
register_fprime_module(Os_File_Stub)
register_fprime_implementation(Os/File Os_File_Stub)

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubFileTests.cpp"
)
set(UT_MOD_DEPS
    Os
)
choose_fprime_implementation(Os/File Os_File_Stub)
register_fprime_ut(StubFileTest)
