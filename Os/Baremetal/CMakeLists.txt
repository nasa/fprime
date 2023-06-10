# Task implementation
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/TaskRunner")
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
)
set(MOD_DEPS Os/Baremetal/TaskRunner)
register_fprime_module(Os_Task_Baremetal)
target_include_directories(Os_Task_Baremetal PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
register_fprime_implementation(Os_Task Os_Task_Baremetal)

register_fprime_module(Os_Queue_Baremetal)
register_fprime_implementation(Os_Task Os_Queue_Baremetal)