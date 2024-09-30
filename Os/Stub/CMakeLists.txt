####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_custom_target("${FPRIME_CURRENT_MODULE}")

######## File Stub ########
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/DefaultFile.cpp"
)
register_fprime_module(Os_File_Stub)
register_fprime_implementation(Os/File Os_File_Stub)

######## Mutex Stub ########
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ConditionVariable.cpp"
)
register_fprime_module(Os_Mutex_Stub)
register_fprime_implementation(Os/Mutex Os_Mutex_Stub "${CMAKE_CURRENT_LIST_DIR}/DefaultMutex.cpp")

#### Queue Stub ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Queue.cpp"
)
set(HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Queue.hpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Queue_Stub)
register_fprime_implementation(Os/Queue Os_Queue_Stub "${CMAKE_CURRENT_LIST_DIR}/DefaultQueue.cpp")

#### Task Stub ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
)
set(HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Task.hpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Task_Stub)
register_fprime_implementation(Os/Task Os_Task_Stub "${CMAKE_CURRENT_LIST_DIR}/DefaultTask.cpp")

#### Console Stub ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Console.cpp"
)
set(HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Console.hpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Console_Stub)
register_fprime_implementation(Os/Console Os_Console_Stub "${CMAKE_CURRENT_LIST_DIR}/DefaultConsole.cpp")

######## FileSystem Stub ########
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/FileSystem.cpp"
)
set(MOD_DEPS)
register_fprime_module(Os_FileSystem_Stub)
register_fprime_implementation(Os/FileSystem Os_FileSystem_Stub "${CMAKE_CURRENT_LIST_DIR}/DefaultFileSystem.cpp")


######## Directory Stub ########
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Directory.cpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Directory_Stub)
register_fprime_implementation(Os/Directory Os_Directory_Stub "${CMAKE_CURRENT_LIST_DIR}/DefaultDirectory.cpp")

# Remainder of file is specific to UTs
if (NOT BUILD_TESTING)
    return()
endif ()

#### File Stub Testing ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/File.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/DefaultFile.cpp"
)
register_fprime_module(Os_File_Test_Stub)
register_fprime_implementation(Os/File Os_File_Test_Stub)

set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubFileTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/FileRules.cpp"
)
set(UT_MOD_DEPS
    Os
    Os_Models
    Os_Test_File_SyntheticFileSystem
    STest
)
choose_fprime_implementation(Os/File Os_File_Test_Stub)
register_fprime_ut(StubFileTest)

#### Console Stub Testing ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/Console.cpp"
)
set(MOD_DEPS)
register_fprime_module(Os_Console_Test_Stub)
register_fprime_implementation(Os/Console Os_Console_Test_Stub "${CMAKE_CURRENT_LIST_DIR}/test/DefaultConsole.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubConsoleTests.cpp"
)
set(UT_MOD_DEPS)
choose_fprime_implementation(Os/Console Os_Console_Test_Stub)
register_fprime_ut(StubConsoleTest)


#### Queue Stub Testing ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/Queue.cpp"
)
register_fprime_module(Os_Queue_Test_Stub)
register_fprime_implementation(Os/Queue Os_Queue_Test_Stub "${CMAKE_CURRENT_LIST_DIR}/test/DefaultQueue.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubQueueTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/queue/QueueRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/queue/CommonTests.cpp"
)
set(UT_MOD_DEPS
        Fw/Time
        Os
        STest
)
choose_fprime_implementation(Os/Queue Os_Queue_Test_Stub)
register_fprime_ut(StubQueueTest)
if (TARGET StubQueueTest)
    target_include_directories(StubQueueTest PRIVATE "${CMAKE_CURRENT_LIST_DIR}/test/ut")
endif ()

#### Task Stub Testing ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/Task.cpp"

)
register_fprime_module(Os_Task_Test_Stub)
register_fprime_implementation(Os/Task Os_Task_Test_Stub "${CMAKE_CURRENT_LIST_DIR}/test/DefaultTask.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubTaskTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/task/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/task/TaskRules.cpp"
)
set(UT_MOD_DEPS
        Fw_Time
        Os
        Os_Models
        STest
)
choose_fprime_implementation(Os/Task Os_Task_Test_Stub)
register_fprime_ut(StubTaskTest)

#### Mutex Stub Testing ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/Mutex.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ConditionVariable.cpp"

)
register_fprime_module(Os_Mutex_Test_Stub)
register_fprime_implementation(Os/Mutex Os_Mutex_Test_Stub "${CMAKE_CURRENT_LIST_DIR}/test/DefaultMutex.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubMutexTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/mutex/CommonTests.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../test/ut/mutex/MutexRules.cpp"
)
set(UT_MOD_DEPS
        Fw_Time
        Os
        Os_Models
        STest
)
choose_fprime_implementation(Os/Task Os_Task_Posix)
choose_fprime_implementation(Os/Mutex Os_Mutex_Test_Stub)
register_fprime_ut(StubMutexTest)

#### FileSystem Stub Testing ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/FileSystem.cpp"
)
register_fprime_module(Os_FileSystem_Test_Stub)
register_fprime_implementation(Os/FileSystem Os_FileSystem_Test_Stub "${CMAKE_CURRENT_LIST_DIR}/test/DefaultFileSystem.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubFileSystemTests.cpp"
)
set(UT_MOD_DEPS
        Fw_Time
        Os
        Os_Models
        STest
)
choose_fprime_implementation(Os/FileSystem Os_FileSystem_Test_Stub)
register_fprime_ut(StubFileSystemTest)

#### Directory Stub Testing ####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/Directory.cpp"
)
register_fprime_module(Os_Directory_Test_Stub)
register_fprime_implementation(Os/Directory Os_Directory_Test_Stub "${CMAKE_CURRENT_LIST_DIR}/test/DefaultDirectory.cpp")

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubDirectoryTests.cpp"
)
set(UT_MOD_DEPS
        Os
        Os_Models
        STest
)
choose_fprime_implementation(Os/Directory Os_Directory_Test_Stub)
register_fprime_ut(StubDirectoryTest)

## Condition variable tests
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubConditionTests.cpp"
)
set(UT_MOD_DEPS
        Fw_Time
)
choose_fprime_implementation(Os/Task Os_Task_Posix)
choose_fprime_implementation(Os/Mutex Os_Mutex_Test_Stub)
register_fprime_ut(StubConditionVariableTest)

## Stubs test: checks that all the no-op stubs can be selected and compile ##
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubTests.cpp"
)
set(UT_MOD_DEPS
        Os
)
choose_fprime_implementation(Os/Task Os_Task_Stub)
choose_fprime_implementation(Os/Mutex Os_Mutex_Stub)
choose_fprime_implementation(Os/File Os_File_Stub)
choose_fprime_implementation(Os/Console Os_Console_Stub)
choose_fprime_implementation(Os/FileSystem Os_FileSystem_Stub)
choose_fprime_implementation(Os/Directory Os_Directory_Stub)
register_fprime_ut(StubTest)
