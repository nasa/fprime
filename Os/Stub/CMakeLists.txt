####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_custom_target("${FPRIME_CURRENT_MODULE}")

add_fprime_supplied_os_module("File;FileSystem;Directory" Stub)
add_fprime_supplied_os_module(Console Stub)
add_fprime_supplied_os_module(Task Stub)
add_fprime_supplied_os_module("Mutex;ConditionVariable" Stub)
add_fprime_supplied_os_module(Cpu Stub)
add_fprime_supplied_os_module(Memory Stub)
add_fprime_supplied_os_module(Queue Stub)

# Remainder of file is specific to UTs
if (NOT BUILD_TESTING)
    return()
endif ()
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")

## Stubs test: checks that all the no-op stubs can be selected and compile ##
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/StubTests.cpp"
)
set(UT_MOD_DEPS Os Os_Models)
choose_fprime_implementation(Os/Task Os_Task_Stub)
choose_fprime_implementation(Os/Mutex Os_Mutex_Stub)
choose_fprime_implementation(Os/File Os_File_Stub)
choose_fprime_implementation(Os/Console Os_Console_Stub)
choose_fprime_implementation(Os/Cpu Os_Cpu_Stub)
choose_fprime_implementation(Os/Memory Os_Memory_Stub)
register_fprime_ut(StubTest)
