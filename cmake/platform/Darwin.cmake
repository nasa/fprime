####
# Darwin.cmake:
#
# Darwin based platform file used for Darwin (Mac OS X) targets. Note: this sets some OS X flags before calling into the common
# Linux implementations to use the posix types defined there.
####

add_definitions(-DTGT_OS_TYPE_DARWIN)

choose_fprime_implementation(Os_Task Os_Task_Posix)
choose_fprime_implementation(Os_Queue Os_Queue_Posix)
choose_fprime_implementation(Os_File Os_File_Posix)
choose_fprime_implementation(Os_Timer Os_Timer_Posix)
choose_fprime_implementation(Os_Log Os_Log_Posix)
choose_fprime_implementation(Os_Mutex Os_Mutex_Posix)
choose_fprime_implementation(Os_Resource Os_Resource_Darwin)
choose_fprime_implementation(Os_Watchdog Os_Resource_Stub)
choose_fprime_implementation(Os_InterruptLock Os_InterruptLock_Stub)

# Set platform default for stubbed drivers
if (NOT DEFINED FPRIME_USE_STUBBED_DRIVERS)
   set(FPRIME_USE_STUBBED_DRIVERS ON)
endif()

# Set platform default for baremetal scheduler drivers
if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads REQUIRED )
endif()

# Add linux include path which is compatible with Darwin for PlatformTypes.hpp
include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/types")
