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
set(MOD_DEPS config)
register_fprime_module(Os_Posix_Shared)

# Set up Posix implementations
add_fprime_supplied_os_module("File;FileSystem;Directory" Posix Os_Posix_Shared)
add_fprime_supplied_os_module(Console Posix)
add_fprime_supplied_os_module(Task Posix Os_Posix_Shared Fw_Time)
add_fprime_supplied_os_module("Mutex;ConditionVariable" Posix Os_Posix_Shared)

# -----------------------------------------
### Os/File/Posix Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/FileRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixFileTests.cpp"
)
add_fprime_os_test(PosixFileTest "${UT_SOURCE_FILES}" "Os_File\;Os_File_Posix" Os_Test_File_SyntheticFileSystem)

# -----------------------------------------
### Os/Console/Posix Test Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixConsoleTests.cpp"
)
add_fprime_os_test(PosixConsoleTest "${UT_SOURCE_FILES}" "Os_Console\;Os_Console_Posix")

# -----------------------------------------
### Os/Task/Posix Test Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/task/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/task/TaskRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixTaskTests.cpp"
)
add_fprime_os_test(PosixTaskTest "${UT_SOURCE_FILES}" "Os_Task\;Os_Task_Posix")

# -----------------------------------------
### Os/Mutex/Posix Test Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/mutex/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/mutex/MutexRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixMutexTests.cpp"
)
add_fprime_os_test(PosixMutexTest "${UT_SOURCE_FILES}" "Os_Mutex\;Os_Mutex_Posix")

# -----------------------------------------
### Os/FileSystem/Posix Test Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/filesystem/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/filesystem/FileSystemRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixFileSystemTests.cpp"
)
add_fprime_os_test(PosixFileSystemTest "${UT_SOURCE_FILES}" "Os_File\;Os_File_Posix")

# -----------------------------------------
### Os/Directory/Posix Test Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/directory/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/directory/DirectoryRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixDirectoryTests.cpp"
)
add_fprime_os_test(PosixDirectoryTest "${UT_SOURCE_FILES}" "Os_File\;Os_File_Posix")

# -----------------------------------------
### Os/ConditionVariable/Posix Test Section
# -----------------------------------------
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/condition/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/condition/ConditionRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixConditionTests.cpp"
)
add_fprime_os_test(PosixConditionTests "${UT_SOURCE_FILES}" "Os_Mutex\;Os_Mutex_Posix")
