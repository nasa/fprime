####
# Linux.cmake:
#
# Linux platform file for standard linux targets. Merely defers to [./Linux-common.cmake](Linux-common.cmake).
####
choose_fprime_implementation(Os_Task Os_Task_Posix)
choose_fprime_implementation(Os_Queue Os_Queue_Posix)
choose_fprime_implementation(Os_File Os_File_Posix)
choose_fprime_implementation(Os_Timer Os_Timer_Posix)
choose_fprime_implementation(Os_Log Os_Log_Posix)
choose_fprime_implementation(Os_Mutex Os_Mutex_Posix)
choose_fprime_implementation(Os_Resource Os_Resource_Linux)
choose_fprime_implementation(Os_Watchdog Os_Resource_Stub)
choose_fprime_implementation(Os_InterruptLock Os_InterruptLock_Stub)


# Set platform default for baremetal scheduler drivers
if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads REQUIRED )
endif()

# Use common linux setup
add_definitions(-DTGT_OS_TYPE_LINUX)
set(FPRIME_USE_POSIX ON)

# Add Linux specific headers into the system
include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/types")

