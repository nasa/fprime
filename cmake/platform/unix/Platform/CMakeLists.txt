####
# unix/Platform/CMakeLists.txt:
#
# This file defines a configuration module for unix platforms.
####
register_fprime_config(
        UnixPlatformTypes
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/PlatformTypes.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/PlatformTypes.h"
    CHOOSES_IMPLEMENTATIONS
        Os_File_Posix
        Os_Console_Posix
        Os_Task_Posix
        Os_Mutex_Posix
        Os_Generic_PriorityQueue
        Os_RawTime_Posix
        Fw_StringFormat_snprintf
        # No posix API
        Os_Cpu_Stub
        Os_Memory_Stub
    INTERFACE # No compilable outputs produced
    BASE_CONFIG
)
