####
# target/test_recursion.cmake:
#
# This target sets up a test target for checking the right recursive dependencies come in
####
include(utilities)
# Current full dependency list for TestDeployment (mostly via Svc_CmdDispatcher)
set(EXPECTED_FULL_DEPENDENCIES Fw Fw_Buffer Fw_Cmd Fw_Com Fw_Comp Fw_CompQueued Fw_Fpy Fw_Log Fw_Logger Fw_Obj Fw_Port
    Fw_Prm Fw_Time Fw_Tlm Fw_Types Os Os_Console_Common Os_Console_Posix Os_Cpu_Common Os_Cpu_${FPRIME_PLATFORM} Os_File_Common
    Os_File_Posix Os_Generic_PriorityQueue Os_Generic_Types Os_Memory_Common Os_Memory_${FPRIME_PLATFORM} Os_Mutex_Common
    Os_Mutex_Posix Os_Posix_Shared Os_RawTime_Common Os_RawTime_Posix Os_Task_Common Os_Task_Posix Svc_CmdDispatcher
    Svc_Ping Svc_Sched TestDeployment TestLibrary2_TestComponent TestLibrary_TestComponent Utils_Hash __fprime_config
    cmake_platform_unix_Platform default_config snprintf-format)

function(test_recursion_add_global_target TARGET)
endfunction(test_recursion_add_global_target)

function(test_recursion_add_deployment_target MODULE TARGET SOURCES DIRECT_DEPENDENCIES FULL_DEPENDENCY_LIST)
    list(SORT FULL_DEPENDENCY_LIST)
    list(SORT EXPECTED_FULL_DEPENDENCIES)
    string(REPLACE ";" ", " EXPECTED_FULL_DEPENDENCIES_SEP "${EXPECTED_FULL_DEPENDENCIES}")
    string(REPLACE ";" ", " FULL_DEPENDENCY_LIST_SEP "${FULL_DEPENDENCY_LIST}")

    fprime_cmake_ASSERT("Expected '${EXPECTED_FULL_DEPENDENCIES_SEP}' got '${FULL_DEPENDENCY_LIST_SEP}'."
                        EXPECTED_FULL_DEPENDENCIES STREQUAL FULL_DEPENDENCY_LIST)
endfunction(test_recursion_add_deployment_target)

function(test_recursion_add_module_target MODULE TARGET SOURCES DEPENDENCIES)
endfunction(test_recursion_add_module_target)
