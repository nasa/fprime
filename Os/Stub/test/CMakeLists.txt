####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
if (NOT BUILD_TESTING)
    return()
endif ()
add_custom_target("${FPRIME_CURRENT_MODULE}")

add_fprime_supplied_os_module("File;FileSystem;Directory" Test_Stub)
add_fprime_supplied_os_module(Console Test_Stub)
add_fprime_supplied_os_module(Task Test_Stub)
add_fprime_supplied_os_module("Mutex;ConditionVariable" Test_Stub)
add_fprime_supplied_os_module(Cpu Test_Stub)
add_fprime_supplied_os_module(Memory Test_Stub)
add_fprime_supplied_os_module(Queue Test_Stub)

#### File Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubFileTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/file/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/file/FileRules.cpp"
)
add_fprime_os_test(StubFileTest "${UT_SOURCE_FILES}" "Os_File\;Os_File_Test_Stub" Os_Test_File_SyntheticFileSystem)

#### Console Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubConsoleTests.cpp"
)
add_fprime_os_test(StubConsoleTest "${UT_SOURCE_FILES}" "Os_Console\;Os_Console_Test_Stub")

#### Cpu Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubCpuTests.cpp"
)
add_fprime_os_test(StubCpuTest "${UT_SOURCE_FILES}" "Os_Cpu\;Os_Cpu_Test_Stub")


#### Memory Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubMemoryTests.cpp"
)
add_fprime_os_test(StubMemoryTest "${UT_SOURCE_FILES}" "Os_Memory\;Os_Memory_Test_Stub")

#### Queue Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubQueueTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/queue/QueueRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/queue/CommonTests.cpp"
)
add_fprime_os_test(StubQueueTest "${UT_SOURCE_FILES}" "Os_Queue\;Os_Queue_Test_Stub")

if (TARGET StubQueueTest)
    target_include_directories(StubQueueTest PRIVATE "${CMAKE_CURRENT_LIST_DIR}/ut")
endif ()

#### Task Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubTaskTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/task/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/task/TaskRules.cpp"
)
add_fprime_os_test(StubTaskTest "${UT_SOURCE_FILES}" "Os_Task\;Os_Task_Test_Stub" Fw_Time)

#### Mutex Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubMutexTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/mutex/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/mutex/MutexRules.cpp"
)
add_fprime_os_test(StubMutexTest "${UT_SOURCE_FILES}" "Os_Mutex\;Os_Mutex_Test_Stub" Fw_Time)

#### FileSystem Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubFileSystemTests.cpp"
)
add_fprime_os_test(StubFileSystemTest "${UT_SOURCE_FILES}" "Os_File\;Os_File_Test_Stub" Fw_Time)

#### Directory Stub Testing ####
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubDirectoryTests.cpp"
)
add_fprime_os_test(StubDirectoryTest "${UT_SOURCE_FILES}" "Os_File\;Os_File_Test_Stub" Fw_Time)

## Condition variable tests
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubConditionTests.cpp"
)
add_fprime_os_test(StubConditionVariableTest "${UT_SOURCE_FILES}" "Os_Mutex\;Os_Mutex_Test_Stub" Fw_Time)
