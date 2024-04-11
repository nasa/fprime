####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Posix)
add_custom_target("${FPRIME_CURRENT_MODULE}")

#### Shared Section ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.hpp"
)
register_fprime_module(Os_Posix_Shared)

#### Os/File/Posix Section ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/File.hpp"
)
set(MOD_DEPS Os_Posix_Shared)
register_fprime_module(Os_File_Posix)
register_fprime_implementation(Os/File Os_File_Posix "${CMAKE_CURRENT_LIST_DIR}/DefaultFile.cpp")

#### Os/File/Posix Unit Tests ####
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/FileRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixFileTests.cpp"
)
set(UT_MOD_DEPS
        Os
        STest
        Os_Test_File_SyntheticFileSystem
)
choose_fprime_implementation(Os/File Os_File_Posix)
register_fprime_ut(PosixFileTest)

#### Os/Task/Posix Section ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.hpp"
)

set(MOD_DEPS Os_Posix_Shared Fw/Time Os)
register_fprime_module(Os_Task_Posix)
register_fprime_implementation(Os/Task Os_Task_Posix "${CMAKE_CURRENT_LIST_DIR}/DefaultTask.cpp")

#### Os/Task/Posix Unit Tests ####
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/task/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/task/TaskRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixTaskTests.cpp"
)
set(UT_MOD_DEPS
        Os
        STest
)
choose_fprime_implementation(Os/Task Os_Task_Posix)
register_fprime_ut(PosixTaskTest)