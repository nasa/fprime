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
    "${CMAKE_CURRENT_LIST_DIR}/errno.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/errno.hpp"
)
register_fprime_module(Os_File_Posix)
register_fprime_implementation(Os/File Os_File_Posix)

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/MyRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixFileTests.cpp"
)
set(UT_MOD_DEPS
    Os
    STest
    Os_Test_File_SyntheticFileSystem
)
choose_fprime_implementation(Os/File Os_File_Posix)
register_fprime_ut(PosixFileTest)