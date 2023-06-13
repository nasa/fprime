
# Task implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/TaskId.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
)

register_fprime_module(Os_Task_Posix)
target_include_directories(Os_Task_Posix PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
register_fprime_implementation(Os_Task Os_Task_Posix)

# File implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileSystem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Directory.cpp"
)

register_fprime_module(Os_File_Posix)
register_fprime_implementation(Os_File Os_File_Posix)

# Queue implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/PthreadQueue.cpp"
)
set(MOD_DEPS Utils/Types/BufferQueue)
register_fprime_module(Os_Queue_Posix)
register_fprime_implementation(Os_Queue Os_Queue_Posix)
set(MOD_DEPS)

# Log implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/LogPrintf.cpp"
)
register_fprime_module(Os_Log_Posix)
register_fprime_implementation(Os_Log Os_Log_Posix)

# Timer implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/IntervalTimer.cpp"
)
register_fprime_module(Os_Timer_Posix)
register_fprime_implementation(Os_Timer Os_Time_Posix)


# Mutex implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp"
)
register_fprime_module(Os_Mutex_Posix)
register_fprime_implementation(Os_Mutex Os_Mutex_Posix)
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Interceptors")