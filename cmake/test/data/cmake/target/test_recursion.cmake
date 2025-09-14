####
# target/test_recursion.cmake:
#
# This target sets up a test target for checking the right recursive dependencies come in
####
include(utilities)
# Current full dependency list for TestDeployment (mostly via Svc_CmdDispatcher)
set(EXPECTED_FULL_DEPENDENCIES
    Fw
    Fw_Buffer
    Fw_Cmd
    Fw_Com
    Fw_Comp
    Fw_CompQueued
    Fw_Fpy
    Fw_Log
    Fw_Logger
    Fw_Obj
    Fw_Port
    Fw_Prm
    Fw_StringFormat_snprintf
    Fw_Time
    Fw_Tlm
    Fw_Types
    Os
    Os_Console
    Os_Console_Posix
    Os_Console_Posix_Implementation
    Os_Cpu
    Os_Cpu_${FPRIME_PLATFORM}
    Os_Cpu_${FPRIME_PLATFORM}_Implementation
    Os_File
    Os_File_Posix
    Os_File_Posix_Implementation
    Os_Generic_PriorityQueue
    Os_Generic_PriorityQueue_Implementation
    Os_Generic_Types
    Os_Memory
    Os_Memory_${FPRIME_PLATFORM}
    Os_Memory_${FPRIME_PLATFORM}_Implementation
    Os_Mutex
    Os_Mutex_Posix
    Os_Mutex_Posix_Implementation
    Os_Posix_Shared
    Os_Queue
    Os_RawTime
    Os_RawTime_Posix
    Os_RawTime_Posix_Implementation
    Os_Task
    Os_Task_Posix
    Os_Task_Posix_Implementation
    Svc_CmdDispatcher
    Svc_Ping
    Svc_Sched
    TestDeployment
    TestLibrary2_TestComponent
    TestLibrary_TestComponent
    UnixPlatformTypes
    Utils_Hash
    __fprime_config
    default_config
)

function(test_recursion_add_global_target TARGET)
endfunction(test_recursion_add_global_target)

function(test_recursion_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
    list(SORT FULL_DEPENDENCY_LIST)
    list(SORT EXPECTED_FULL_DEPENDENCIES)
    string(REPLACE ";" "\n    " EXPECTED_FULL_DEPENDENCIES_SEP "${EXPECTED_FULL_DEPENDENCIES}")
    string(REPLACE ";" "\n    " FULL_DEPENDENCY_LIST_SEP "${FULL_DEPENDENCY_LIST}")
    # Write lists to file
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/test_recursion_expected.txt" "${EXPECTED_FULL_DEPENDENCIES_SEP}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/test_recursion_found.txt" "${FULL_DEPENDENCY_LIST_SEP}")
    execute_process(COMMAND diff
            "${CMAKE_CURRENT_BINARY_DIR}/test_recursion_expected.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/test_recursion_found.txt"
        OUTPUT_VARIABLE DIFF_OUTPUT
        RESULT_VARIABLE DIFF_RESULT
    )
    fprime_cmake_ASSERT("diff (Expected vs Found:\n${DIFF_OUTPUT}" DIFF_RESULT EQUAL 0)
endfunction(test_recursion_add_deployment_target)

function(test_recursion_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
endfunction(test_recursion_add_module_target)
