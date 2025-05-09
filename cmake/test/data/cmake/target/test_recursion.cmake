####
# target/test_recursion.cmake:
#
# This target sets up a test target for checking the right recursive dependencies come in
####
include(utilities)
# Current full dependency list for TestDeployment (mostly via Svc_CmdDispatcher)
set(EXPECTED_FULL_DEPENDENCIES TestDeployment Svc_CmdDispatcher config cmake_platform_unix Fw_Cmd Fw_Com Fw_Time
    Fw_Port Fw_Cfg Fw_Types Fw_StringFormat Fw_Logger Fw_Obj Fw_Log Fw_Tlm Fw_Fpy Svc_Ping Fw Fw_Prm Fw_CompQueued Os
    Os_Console_Common Os_File_Common Utils_Hash Os_Task_Common Os_Mutex_Common Os_Queue_Common Os_Cpu_Common
    Os_Memory_Common Os_RawTime_Common Fw_Buffer Os_Console Os_Cpu Os_File Os_Memory Os_Mutex Os_Queue Os_RawTime
    Os_Task Fw_Comp TestLibrary_TestComponent Svc_Sched TestLibrary2_TestComponent)

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
