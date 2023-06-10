# File implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileSystem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Directory.cpp"
)

register_fprime_module(Os_File_Stub)
register_fprime_implementation(Os_File Os_File_Stub)

# Timer implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/IntervalTimer.cpp"
)
register_fprime_module(Os_Timer_Stub)
register_fprime_implementation(Os_Timer Os_Time_Stub)


# Mutex implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp"
)
register_fprime_module(Os_Mutex_Stub)
register_fprime_implementation(Os_Mutex Os_Mutex_Stub)

# SystemResources implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/SystemResources.cpp"
)

register_fprime_module(Os_Resource_Stub)
register_fprime_implementation(Os_Resource Os_Resource_Stub)

# Watchdog implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/WatchdogTimer.cpp"
)

register_fprime_module(Os_Watchdog_Stub)
register_fprime_implementation(Os_Watchdog Os_Watchdog_Stub)

# InterruptLock implementation
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/InterruptLock.cpp"
)

register_fprime_module(Os_InterruptLock_Stub)
register_fprime_implementation(Os_InterruptLock Os_InterruptLock_Stub)