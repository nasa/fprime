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

add_compile_options(
    -Wno-conversion
)

register_os_implementation("File;FileSystem;Directory" Test_Stub)
register_os_implementation(Console Test_Stub)
register_os_implementation(Task Test_Stub)
register_os_implementation("Mutex;ConditionVariable" Test_Stub)
register_os_implementation(Cpu Test_Stub)
register_os_implementation(Memory Test_Stub)
register_os_implementation(Queue Test_Stub)
register_os_implementation(RawTime Test_Stub) # add Fw_Buffer here?

#### File Stub Testing ####
register_fprime_ut(
    StubFileTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubFileTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/file/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/file/FileRules.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_File_Test_Stub
  DEPENDS
    Fw_Types
    Os
    Os_Test_File_SyntheticFileSystem
    STest
)

#### Console Stub Testing ####
register_fprime_ut(
    StubConsoleTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubConsoleTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Console_Test_Stub
  DEPENDS
    Fw_Types
    STest
)


#### Cpu Stub Testing ####
register_fprime_ut(
    StubCpuTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubCpuTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Cpu_Test_Stub
  DEPENDS
    Fw_Types
    STest
)

#### Memory Stub Testing ####
register_fprime_ut(
    StubMemoryTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubMemoryTests.cpp"
  CHOOSES_IMPLEMENTATIONS
  Os_Memory_Test_Stub
  DEPENDS
    Fw_Types
    STest
)

#### Queue Stub Testing ####
register_fprime_ut(
    StubQueueTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubQueueTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/queue/QueueRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/queue/CommonTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Queue_Test_Stub
  DEPENDS
    Os
    Fw_Types
    STest
)
if (TARGET StubQueueTest)
    target_include_directories(StubQueueTest PRIVATE "${CMAKE_CURRENT_LIST_DIR}/ut")
endif ()

#### Task Stub Testing ####
register_fprime_ut(
    StubTaskTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubTaskTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/task/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/task/TaskRules.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Task_Test_Stub
  DEPENDS
    Fw_Types
    STest
    Os
)

#### Mutex Stub Testing ####
register_fprime_ut(
    StubMutexTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubMutexTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/mutex/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../test/ut/mutex/MutexRules.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Mutex_Test_Stub
  DEPENDS
    Fw_Types
    STest
)

#### FileSystem Stub Testing ####
register_fprime_ut(
    StubFileSystemTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubFileSystemTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_File_Test_Stub
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)

#### Directory Stub Testing ####
register_fprime_ut(
    StubDirectoryTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubDirectoryTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_File_Test_Stub
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)

## Condition variable tests
register_fprime_ut(
    StubConditionVariableTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubConditionTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_Mutex_Test_Stub
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)

#### RawTime Stub Testing ####
register_fprime_ut(
    StubRawTimeTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ut/StubRawTimeTests.cpp"
  CHOOSES_IMPLEMENTATIONS
    Os_RawTime_Test_Stub
  DEPENDS
    Fw_Types
    Fw_Time
    STest
)