####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_custom_target("${FPRIME_CURRENT_MODULE}")

register_os_implementation("File;FileSystem;Directory" Stub)
register_os_implementation(Console Stub)
register_os_implementation(Task Stub)
register_os_implementation("Mutex;ConditionVariable" Stub)
register_os_implementation(Cpu Stub)
register_os_implementation(Memory Stub)
register_os_implementation(Queue Stub)
register_os_implementation(RawTime Stub)

register_fprime_implementation(
    Os_File_None
  IMPLEMENTS
    Os_File
  INTERFACE
)

# Remainder of file is specific to UTs
if (NOT BUILD_TESTING)
    return()
endif ()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")

## Stubs test: checks that all the no-op stubs can be selected and compile ##
register_fprime_ut(
    StubTest
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubTests.cpp"
  DEPENDS
    Fw_Types
    Fw_Time
    Os
    Os_Models
  CHOOSES_IMPLEMENTATIONS
    Os_Task_Stub
    Os_Mutex_Stub
    Os_File_Stub
    Os_Console_Stub
    Os_Cpu_Stub
    Os_Memory_Stub
    Os_RawTime_Stub
)
if (TARGET StubTest)
    target_compile_options(StubTest PRIVATE -Wno-conversion)
endif()
