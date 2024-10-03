####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Posix)
add_custom_target("${FPRIME_CURRENT_MODULE}")

# -----------------------------------------
## Shared Section
# -----------------------------------------

set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.hpp"
)
register_fprime_module(Os_Posix_Shared)

# -----------------------------------------
### Os/File/Posix Section
# -----------------------------------------
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

# -----------------------------------------
### Os/Console/Posix Section
# -----------------------------------------
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Console.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Console.hpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Console_Posix)
register_fprime_implementation(Os/Console Os_Console_Posix "${CMAKE_CURRENT_LIST_DIR}/DefaultConsole.cpp")

#### Os/Console/Posix Unit Tests ####
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixConsoleTests.cpp"
)
set(UT_MOD_DEPS
        Os
)
choose_fprime_implementation(Os/Console Os_Console_Posix)
register_fprime_ut(PosixConsoleTest)


# -----------------------------------------
### Os/Task/Posix Section
# -----------------------------------------

set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.hpp"
)

set(MOD_DEPS Os_Posix_Shared Fw/Time Os) # Os needs to be removed here?
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

# -----------------------------------------
### Os/Mutex/Posix Section
# -----------------------------------------
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ConditionVariable.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Mutex.hpp"
)
set(MOD_DEPS Os_Posix_Shared)
register_fprime_module(Os_Mutex_Posix)
register_fprime_implementation(Os/Mutex Os_Mutex_Posix "${CMAKE_CURRENT_LIST_DIR}/DefaultMutex.cpp")

#### Os/Mutex/Posix Unit Tests ####
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/mutex/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/mutex/MutexRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixMutexTests.cpp"
)
set(UT_MOD_DEPS
        Os
        STest
)
choose_fprime_implementation(Os/Mutex Os_Mutex_Posix)
register_fprime_ut(PosixMutexTest)

# -----------------------------------------
### Os/FileSystem/Posix Section
# -----------------------------------------
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/FileSystem.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/FileSystem.hpp"
)
set(MOD_DEPS Os_Posix_Shared)
register_fprime_module(Os_FileSystem_Posix)
register_fprime_implementation(Os/FileSystem Os_FileSystem_Posix "${CMAKE_CURRENT_LIST_DIR}/DefaultFileSystem.cpp")

#### Os/FileSystem/Posix Unit Tests ####
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/filesystem/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/filesystem/FileSystemRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixFileSystemTests.cpp"
)
set(UT_MOD_DEPS
        Os
        STest
)
choose_fprime_implementation(Os/FileSystem Os_FileSystem_Posix)
register_fprime_ut(PosixFileSystemTest)

# -----------------------------------------
### Os/Directory/Posix Section
# -----------------------------------------
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Directory.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Directory.hpp"
)
set(MOD_DEPS Os_Posix_Shared)
register_fprime_module(Os_Directory_Posix)
register_fprime_implementation(Os/Directory Os_Directory_Posix "${CMAKE_CURRENT_LIST_DIR}/DefaultDirectory.cpp")

#### Os/Directory/Posix Unit Tests ####
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/directory/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/directory/DirectoryRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixDirectoryTests.cpp"
)
set(UT_MOD_DEPS
        Os
        STest
)
choose_fprime_implementation(Os/Directory Os_Directory_Posix)
register_fprime_ut(PosixDirectoryTest)

# -----------------------------------------
### Os/ConditionVariable/Posix Section
# -----------------------------------------
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/condition/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/condition/ConditionRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixConditionTests.cpp"
)
choose_fprime_implementation(Os/Mutex Os_Mutex_Posix)
register_fprime_ut(PosixConditionTests)
