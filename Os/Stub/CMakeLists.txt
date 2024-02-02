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
register_fprime_module(Os_File_Stub_Implementation)

set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/DefaultFile.cpp"
)
set(MOD_DEPS
    Os_File_Stub_Implementation
)
register_fprime_module(Os_File_Stub)
register_fprime_implementation(Os/File Os_File_Stub)

if (BUILD_TESTING)
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/File.cpp"
    )
    register_fprime_module(Os_File_Test_Stub_Implementation)

    set(SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/test/DefaultFile.cpp"
    )
    set(MOD_DEPS
            Os_File_Test_Stub_Implementation
    )
    register_fprime_module(Os_File_Test_Stub)

    register_fprime_implementation(Os/File Os_File_Test_Stub)

    set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubFileTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/MyRules.cpp"
    )
    set(UT_MOD_DEPS
        Os
        Os_Test_File_SyntheticFileSystem
        STest
    )
    choose_fprime_implementation(Os/File Os_File_Test_Stub)
    register_fprime_ut(StubFileTest)
endif()